#include "RtspSmssH265.h"
#include "H265VideoStreamFramer.hh"
#include "Base64.hh"

using namespace NAMESPACE_ICATCH_PLUGIN::rtsp;
#define VPS_NUT         32
#define SPS_NUT         33
#define PPS_NUT         34

DEFINE_REGISTER(RtspSmssH265) {
	TClass<UsageEnvironment*,PacketRing*>::Register<RtspSmssH265>(
		name ? name : "rtsp-smss-h265-hevc-live555", {
		})->Init(LIBRARY, VERSION(0), "iCatchtek", "jg.wang");
}
RtspSmssH265::RtspSmssH265(UsageEnvironment *env,PacketRing* ring)
	:RtspSmss(env,ring)
{
	_type="video";
	_codec="H265";
	_rate=90000;
	_channels=1;
}
bool RtspSmssH265::Parse(){
	dbgErrPL();
	IMedia* media = _ring->Media();
	returnIfErrC(false,!media);
	returnIfErrC(false,!media->Get("vps",_vps));
	returnIfErrC(false,!media->Get("ivps",_ivps));
	returnIfErrC(false,!media->Get("pps",_pps));
	returnIfErrC(false,!media->Get("ipps",_ipps));
	returnIfErrC(false,!media->Get("sps",_sps));
	returnIfErrC(false,!media->Get("isps",_isps));
	returnIfErrC(false,!_pps);
	returnIfErrC(false,!_ipps);
	returnIfErrC(false,!_sps);
	returnIfErrC(false,!_isps);
	returnIfErrC(false,!_vps);
	returnIfErrC(false,!_ivps);
	return true;
}

const char* RtspSmssH265::getAuxSDPLine(RTPSink*sink,FramedSource*) {
	dbgErrPL();
	uint8 payloadtype = sink->rtpPayloadType();
	if(_auxsdp) return _auxsdp;
	returnIfErrC(0, !Parse());
	if (! _vps ||! _ivps ||! _sps ||! _isps ||! _pps ||! _ipps){
		dbgErr("no vps, sps or pps");
		return 0;
	}
	dbgDump(_vps,_ivps);
	dbgDump(_sps,_isps);
	dbgDump(_pps,_ipps);
	dbgInfo("had vps & sps & pps");
	uint8* pvps=_vps;//+4;
	uint8* psps=_sps;//+4;
	uint8* ppps=_pps;//+4;
	int ivps=_ivps;//-4;
	int isps=_isps;//-4;
	int ipps=_ipps;//-4;

	// Set up the "a=fmtp:" SDP line for this stream.
	uint8* vpsWEB = new uint8[ivps]; // "WEB" means "Without Emulation Bytes"
	unsigned vpsWEBSize = removeH264or5EmulationBytes(vpsWEB, ivps, pvps, ivps);
	if (vpsWEBSize < 6/*'profile_tier_level' offset*/ + 12/*num 'profile_tier_level' bytes*/) {

		// Bad VPS size => assume our source isn't ready
		delete[] vpsWEB;
		return NULL;

	}

	uint8 const* profileTierLevelHeaderBytes = &vpsWEB[6];
	unsigned profileSpace  = profileTierLevelHeaderBytes[0]>>6; // general_profile_space
	unsigned profileId = profileTierLevelHeaderBytes[0]&0x1F; // general_profile_idc
	unsigned tierFlag = (profileTierLevelHeaderBytes[0]>>5)&0x1; // general_tier_flag
	unsigned levelId = profileTierLevelHeaderBytes[11]; // general_level_idc
	uint8 const* interop_constraints = &profileTierLevelHeaderBytes[5];
	char interopConstraintsStr[100];
	sprintf(interopConstraintsStr, "%02X%02X%02X%02X%02X%02X",
			interop_constraints[0], interop_constraints[1], interop_constraints[2],
			interop_constraints[3], interop_constraints[4], interop_constraints[5]);
	delete[] vpsWEB;

	dbgDump(pvps,ivps);
	dbgDump(psps,isps);
	dbgDump(ppps,ipps);
	char* vps_base64 = base64Encode((char*)pvps, ivps);
	char* sps_base64 = base64Encode((char*)psps, isps);
	char* pps_base64 = base64Encode((char*)ppps, ipps);

	char const* fmtpFmt =
			"a=fmtp:%d profile-space=%u"
					";profile-id=%u"
					";tier-flag=%u"
					";level-id=%u"
					";interop-constraints=%s"
					";sprop-vps=%s"
					";sprop-sps=%s"
					";sprop-pps=%s\r\n";
	unsigned fmtpFmtSize = strlen(fmtpFmt)
						   + 3 /* max num chars: rtpPayloadType */ + 20 /* max num chars: profile_space */
						   + 20 /* max num chars: profile_id */
						   + 20 /* max num chars: tier_flag */
						   + 20 /* max num chars: level_id */
						   + strlen(interopConstraintsStr)
						   + strlen(vps_base64)
						   + strlen(sps_base64)
						   + strlen(pps_base64);
	_auxsdp = new char[fmtpFmtSize];
	sprintf(_auxsdp, fmtpFmt,
			payloadtype, profileSpace,
			profileId,
			tierFlag,
			levelId,
			interopConstraintsStr,
			vps_base64,
			sps_base64,
			pps_base64);

	delete[] vps_base64;
	delete[] sps_base64;
	delete[] pps_base64;

	return _auxsdp;
}

bool RtspSmssH265::FramePreProcess(uint8*&ptr,int& len){
	if(len<6)return false;
	do{
		if(!ptr[0]&&!ptr[1]&&!ptr[2]&&ptr[3]==1){
			uint8 nal=(ptr[4]& 0x7E)>>1;
			if(nal<10 || (nal>15 && nal<22)) {
				_nal = ptr[4]+(ptr[5]<<8);
				len-=6;
				ptr+=6;
				_rtp1st = true;
				return true;
			}
		}
		ptr++;
	}while(len--);

	return true;
}

int RtspSmssH265::RtpHeadUpdate(uint8*ptr,int& len){
	if(_rtp1st && len<=_packsize-2-_headsize){///only one, for nal size < mtu,
		ptr[0] = _nal&0xff;
		ptr[1] = (_nal>>8)&0xff;
		dbgLogPVL(len);
		return 2;
	}
	/**
	 * h265's nalu is different from h264.
	 * FU Header
	 */
	uint8 nalh=_nal;
	/**
	 * create the HEVC payload header and transmit the buffer as fragmentation units (FU)
	 * 0                   1
	 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |F|   Type    |  LayerId  | TID |
	 * +-------------+-----------------+
	 * 		F       = 0
	 * 		Type    = 49 (fragmentation unit (FU))
	 * 		LayerId = 0
	 * 		TID     = 1
     */
	ptr[0] = 49<<1;
	ptr[1] = 1;
	/**
	 * create the FU header
	 * 0 1 2 3 4 5 6 7
	 * +-+-+-+-+-+-+-+-+
	 * |S|E|  FuType   |
	 * +---------------+
	 * 		S       = variable
	 * 		E       = variable
	 * 		FuType  = NAL unit type
	 */
	ptr[2] = (nalh & 0x7E) >> 1;
	if(_rtp1st){///first
		ptr[2] |= 0x80; /** S, Start: means the first package of the slice */
		_rtp1st=false;
	}else if(len<=_packsize-3-_headsize) {///last
		ptr[2] |= 0x40; /** E, End: means the last package of the slice */
	}
	return 3;
}
