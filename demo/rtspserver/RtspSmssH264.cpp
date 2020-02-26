#include "MediaInfo.h"
#include "RtspSmssH264.h"
#include "Base64.hh"
#include "IH264Source.h"
#include "_hash.h"
#include "OmfDbg.h"
///
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntryGreen(s)
///
using namespace omf::rtsp;
using namespace omf::api;
using namespace omf::api::streaming;
using namespace omf::api::streaming::common;
#define H264_IDR  5
#define H264_PDR  1
#define H264_SPS  7
#define H264_PPS  8

//DEFINE_REGISTER(RtspSmssH264) {
//	TClass<UsageEnvironment*,PacketRing*>::Register<RtspSmssH264>(
//		name ? name : "rtsp-smss-h264-live555", {
//		})->Init(LIBRARY, VERSION(0), "iCatchtek", "jg.wang");
//}
RtspSmssH264::RtspSmssH264(UsageEnvironment&env,std::shared_ptr<OmfAttrSet>attr)
	:RtspSmss(env,attr)
	,_jump(0)
{//dbgTestPL();
	//_type="video";
	//_codec="H264";
	//_rate=90000;
	//_channels=1;
	init("video","H264",90000,1);
}
bool RtspSmssH264::Parse(){dbgTestPVL(_codec);
	auto src= dynamic_cast<IH264Source*>(_src);
	returnIfErrC(false,!src);
	auto info=src->GetH264MediaInfo();
	_pps = (uint8*)info.pps;
	_ipps = info.ppsLength;
	_sps = (uint8*)info.sps;
	_isps = info.spsLength;
	dbgTestPVL(info.pack);
	switch(::Hash(info.pack)){
		case ::Hash("avc"):
			_jump = info.cbJumpToDataAvc;
			break;
		case ::Hash("bitstream"):
		case ::Hash("bs"):
			_jump = info.cbJumpToDataBs;
			break;
		default:
			dbgErrPVL(info.pack);
			break;
	};
	returnIfErrC(false,!_pps);
	returnIfErrC(false,!_ipps);
	returnIfErrC(false,!_sps);
	returnIfErrC(false,!_isps);
	returnIfErrC(false,!(void*)_jump);
#if 0
	IMedia* media = _ring->Media();
	returnIfErrC(false,!media);
	//returnIfErrC(false,!media->Get("profile",_profile));
	//returnIfErrC(false,!media->Get("level",_level));
	//returnIfErrC(false,!media->Get("bitrate",_br));
	//returnIfErrC(false,!media->Get("gop",_gop));
	returnIfErrC(false,!media->Get("pps",_pps));
	returnIfErrC(false,!media->Get("ipps",_ipps));
	returnIfErrC(false,!media->Get("sps",_sps));
	returnIfErrC(false,!media->Get("isps",_isps));
	returnIfErrC(false,!media->Get("jump_bs",_jump));
	returnIfErrC(false,!_pps);
	returnIfErrC(false,!_ipps);
	returnIfErrC(false,!_sps);
	returnIfErrC(false,!_isps);
	returnIfErrC(false,!_jump);
#endif
	returnIfErrC(0,!_src->ChangeUp(State::play));
	return true;
}

const char* RtspSmssH264::getAuxSDPLine(RTPSink*sink,FramedSource*) {
	uint8 payloadtype = sink->rtpPayloadType();
	if(_auxsdp) return _auxsdp;
	returnIfErrC(0, !Parse());
	if (! _sps ||! _isps ||! _pps ||! _ipps ){
		dbgErrPSL("no sps or pps");
		return 0;
	}
	dbgNoteDL(_sps,_isps);
	dbgNoteDL(_pps,_ipps);
	dbgTestPSL("had sps & pps");
	uint8* psps=_sps;//+4;
	uint8* ppps=_pps;//+4;
	int isps=_isps;//-4;
	int ipps=_ipps;//-4;
	uint32 profileLevelId = (psps[1]<<16) | (psps[2]<<8) | psps[3];

	char* sps_base64 = base64Encode((char*)psps, isps);
	char* pps_base64 = base64Encode((char*)ppps, ipps);

	char const* fmtpFmt =
		"a=fmtp:%d packetization-mode=1"
			";profile-level-id=%06X"
			";sprop-parameter-sets=%s,%s\r\n";
	//unsigned fmtpFmtSize = strlen(fmtpFmt)
	//	+ 3 /* max char len */
	//	+ 6 /* 3 bytes in hex */
	//	+ strlen(sps_base64) + strlen(pps_base64);
	//char* fmtp = new char[fmtpFmtSize];

	_auxsdp = new char[256];
	memset(_auxsdp,0,256);
	sprintf(_auxsdp, fmtpFmt,
		payloadtype,
		profileLevelId,
		sps_base64, pps_base64);

	delete[] sps_base64;
	delete[] pps_base64;

	dbgTestPVL(_auxsdp);
	return _auxsdp;
}

bool RtspSmssH264::FramePreProcess(uint8*&ptr,int& len){
	//dbgDump(ptr,5);
	returnIfErrC(false,!_jump(ptr,len));
	_rtp1st = true;
	_nal = *ptr;
	//dbgTestVL((void*)_nal);
	ptr++;
	len--;
	return true;
}

int RtspSmssH264::RtpHeadUpdate(uint8*ptr,int& len){
	if(_rtp1st && len<=_packsize-1-_headsize){///only one, for nal size < mtu,
		ptr[0] = _nal;
		//dbgTestPSL(len);
		return 1;
	}
	//FU Header
	uint8 nalh=_nal;
	ptr[0] = (nalh & 0xE0) | 0x1c;
	ptr[1] = (nalh & 0x1f);
	if(_rtp1st){///first
		ptr[1] |= 0x80;
		_rtp1st=false;
	}else if(len<=_packsize-2-_headsize) {///last
		ptr[1] |= 0x40;
	}
	return 2;
}
bool RtspSmssH264::CreateSource(){dbgTestPL();
	auto src=IH264Source::CreateNew("dualos-vbrc-pull");
	returnIfErrC(false,!src);
	_src=src;
	if(_attr){dbgTestPL();
		auto gop = _attr->GetOrDef("gop",15);dbgTestPVL(gop);
		src->SetGop(gop);
		auto goptype = _attr->GetOrDef("goptype","ippp");dbgTestPVL(goptype);
		src->SetGopType(goptype);
		auto w = _attr->GetOrDef("w",1920);dbgTestPVL(w);
		src->SetWidth(w);
		auto h = _attr->GetOrDef("h",1080);dbgTestPVL(h);
		src->SetHeight(h);
		auto br = _attr->GetOrDef("br",1024*1024*1);dbgTestPVL(br);
		src->SetBitRate(br);
		auto fr = _attr->GetOrDef("fr",30);dbgTestPVL(fr);
		src->SetFrameRate(fr);
	}
	return true;
}
