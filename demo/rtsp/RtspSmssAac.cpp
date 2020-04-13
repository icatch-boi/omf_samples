#include "MediaInfo.h"
#include "RtspSmssAac.h"
#include "IAacSource.h"
#include "OmfDbg.h"
using namespace omf::rtsp;
using namespace omf::api::streaming;
using namespace omf::api::streaming::common;
///
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntryGreen(s)
///
//DEFINE_REGISTER(RtspSmssAac) {
//	TClass<UsageEnvironment*,PacketRing*>::Register<RtspSmssAac>(
//		name ? name : "rtsp-smss-aac-live555", {
//		})->Init(LIBRARY, VERSION(0), "iCatchtek", "jg.wang");
//}
RtspSmssAac::RtspSmssAac(UsageEnvironment& env,std::shared_ptr<OmfAttrSet>attr)
	:RtspSmss(env,attr)
{dbgTestPL();
	//_type="audio";
	//_codec="MP4A-LATM";
	init("audio","MP4A-LATM",0,0);
}
bool RtspSmssAac::Parse(){dbgTestPVL(_codec);
	auto src= dynamic_cast<IAacSource*>(_src);
	returnIfErrC(false,!src);
	auto info=src->GetAacMediaInfo();
	_rate=info.rate;
	_channels=info.channels;
	_version=info.version;
	_profile=info.profile;
	_adtslen=info.adtsLength;
	_rateidx=info.rateidx;
	_blocknum = 0;
	dbgTestPVL(_rate);
	dbgTestPVL(_channels);
	dbgTestPVL(_version);
	dbgTestPVL(_profile);
	dbgTestPVL(_adtslen);
	dbgTestPVL(_rateidx);
	return true;
}

const char* RtspSmssAac::getAuxSDPLine(RTPSink*sink,FramedSource*) {
	uint8 payloadtype = sink->rtpPayloadType();
	if(_auxsdp) return _auxsdp;
	returnIfErrC(0, !Parse());
	char data[4]={0,0,0,0};
	//memset(data,0,sizeof(data));
	/* Create StreamMuxConfig according to ISO/IEC 14496-3:
	*
	* audioMuxVersion           == 0 (1 bit)
	* allStreamsSameTimeFraming == 1 (1 bit)
	* numSubFrames              == numSubFrames (6 bits)
	* numProgram                == 0 (4 bits)
	* numLayer                  == 0 (3 bits)
	*/
	data[0] = (uint8)(0x40|_blocknum);//0x48;
	data[1] = (uint8)(0x00|(_profile>>4));
	data[2] = (uint8)((_profile<<4) | _rateidx);
	data[3] = (uint8)(_channels<<4);
	//data[2] =(_profile<<3) | (_sampleRateIdx>>1);//8
	//data[3] =((_sampleRateIdx&1)<<7) | (_channels<<3);//5
	//int offset = 15;
	//offset=bsWrite((uint8*)data,offset,_profile,5);
	//offset=bsWrite((uint8*)data,offset,_rateIdx,4);
	//offset=bsWrite((uint8*)data,offset,_channels,4);

	//char const* fmtpFmt =
	//	"a=fmtp:%d "
	//		"cpresent=0;config=%02x%02x%02x%02x\r\n";

	_auxsdp = new char[64];
	sprintf(_auxsdp,
		"a=fmtp:%d "
			"cpresent=0;config=%02x%02x%02x%02x\r\n"
			,payloadtype
			,data[0]
			,data[1]
			,data[2]
			,data[3]
	);
	//_prints(fmtp);
	return _auxsdp;
}

bool RtspSmssAac::FramePreProcess(uint8*&data,int&size){
	//dbgDump(data,8);
	if(_adtslen){
		size-=_adtslen;
		data+=_adtslen;
	}
	_rtp1st = true;
	return true;
}

int RtspSmssAac::RtpHeadUpdate(uint8*ptr,int& len){
	if(_rtp1st){
		_rtp1st = false;
		int size = len;
		while(size>=0xff) {
			*ptr++ = 0xff;
			size-=0xff;
		}
		*ptr++ = size;
		return (len+0xff)/0xff;
	}
	return 0;
}
bool RtspSmssAac::CreateSource(){
	std::string keywords="dualos";
	if(_attr)_attr->Get("keywords",keywords);
	keywords += "-pull";
	dbgTestPVL(keywords);
	auto src=IAacSource::CreateNew(keywords.c_str());
	returnIfErrC(false,!src);
	_src=src;
	if(_attr){
		auto rate = _attr->GetOrDef("rate",0);dbgTestPVL(rate);
		src->SetSampleRate(rate);
		auto ch = _attr->GetOrDef("ch",0);dbgTestPVL(ch);
		src->SetTargetChannels(ch);
		auto bw = _attr->GetOrDef("bw",22000);dbgTestPVL(bw);
		src->SetBandWidth(bw);
		auto br = _attr->GetOrDef("br",128000);dbgTestPVL(br);
		src->SetBitRate(br);
	}
	return true;
}