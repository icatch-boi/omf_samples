#include "RtspSmss.h"
#include "RtpSource.h"
#include "RtpSink.h"
#include "_rtp.h"
#include "OmfDbg.h"
#include "omf_api.h"
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
using namespace omf::rtsp;
using namespace omf::api::streaming;
using namespace omf::api::streaming::common;
RtspSmss::RtspSmss(UsageEnvironment& env,std::shared_ptr<OmfAttrSet>attr)
	:OnDemandServerMediaSubsession(env, true)
	,_attr(attr)
	,_auxsdp(0)
	,_packsize(1460)
	,_payloadtype(0)
	,_rate(0)
	,_channels(0)
	,_type(0)
	,_codec(0)
	//,_copy(memcpy)
	,_headsize(RTP_HEADER_LEN)
{dbgTestPL();
	//dbgTestPSL(Name());
}

RtspSmss::~RtspSmss() {dbgTestPSL(_codec);
	if(_auxsdp)delete [] _auxsdp;
	//if(_attr)delete _attr;
	if(_src)delete _src;
}
bool RtspSmss::init(const char* type
		,const char* codec
		,int rate
		,int channels
){
	if(_src)return true;
	_rate=rate;
	_channels=channels;
	_type=type;
	_codec=codec;

	dbgTestPSL(_codec);
	returnIfErrC(0,!CreateSource());
	returnIfErrC(0,!_src);
	returnIfErrC(0,!_src->ChangeUp(State::ready));
	returnIfErrC(0,!Parse());
}
FramedSource* RtspSmss::createNewStreamSource(unsigned, unsigned& estBitrate) {dbgTestPL();
	estBitrate = 1000;///KBit
	dbgTestPSL(_codec);
	return new RtpSource(envir(),_src);
}

RTPSink *RtspSmss::createNewRTPSink(Groupsock*gs,unsigned char rpf,FramedSource*){
	return new RtpSink(envir(),gs,*this,_payloadtype?_payloadtype:rpf);
}
bool RtspSmss::FramePreProcess(uint8*&ptr,int& size){return true;}
int RtspSmss::RtpHeadUpdate(uint8*ptr,int& size){return 0;}
char const *RtspSmss::getAuxSDPLine(RTPSink*sink,FramedSource*){ return  "";};
//const char* RtspSmss::AuxSdp(uint8 payloadtype){return _auxsdp;}
