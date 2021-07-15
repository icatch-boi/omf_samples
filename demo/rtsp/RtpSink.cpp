#include "RtpSink.h"
#include "rtp.h"
#include <thread>
#include "rtsp.h"
#include "OmfDbg.h"
///////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntryGreen(s)
/////////////////////////////////////////////
using namespace omf::rtsp;
RtpSink::RtpSink(UsageEnvironment& env
	, Groupsock* RTPgs
	, RtspSmss& smss
	, unsigned char rpf
)
	: RTPSink(env,RTPgs,rpf,smss._rate,smss._codec,smss._channels)
	, _smss(smss)
	//, _memcpy(memcpy)
{//dbgNotePVL(smss._codec);dbgNotePVL(smss._rate);dbgNotePVL(smss._channels);dbgNotePVL((unsigned)rpf);
	//_buf=(uint8_t*)new uint32[1500>>2];
	memset(_buff,0,1500);
}
RtpSink::~RtpSink(){dbgTestPVL(_smss._codec);
}
///////////////////////////////////////////////////////////////////////////
Boolean RtpSink::continuePlaying(){
	dbgTestPL();
	InitBuff();
	fSource->getNextFrame(0,0
		,0,0//(ISink*)this
		,onSourceClosure,this);
	_src = (RtpSource*)fSource;
	//_time =  _timepoint_zero;
	_1st = true;
	_rtpBaseTime=std::chrono::steady_clock::duration::zero();
	//Loop();
	nextTask() = envir().taskScheduler().scheduleDelayedTask(1000, (TaskFunc *) cbLoop, this);
	return true;
}

char const* RtpSink::sdpMediaType() const{
	return _smss._type;
}

char const*  RtpSink::auxSDPLine(){
	return _smss.getAuxSDPLine(this,NULL);
}

///////////////////////////////////////////////////////////////////////////
void RtpSink::InitBuff(){
	uint8* ptr = _buff;
	ptr[0]=ptr[1]=0;
	uint32 ssrc=SSRC();
	RTP_HEADER_VERSION (ptr)	= 2; 
	RTP_HEADER_PAYLOAD_TYPE(ptr)= fRTPPayloadType;	
	RTP_HEADER_SSRC (ptr)		= Swap(ssrc);
}
void RtpSink::InitBuff(Packet* pkt){
	//if(_smss._session->_clock0==std::chrono::steady_clock::time_point::min()){
	//	_smss._session->_clock0=pkt->pts;
	//}
	//struct timeval tv = toTimeVal(pkt->pts-_smss._session->_clock0);
	//uint32 time=convertToRTPTimestamp(tv);
	//auto pts = pkt->pts-_smss._session->_clock0;
	uint32 time=ConvertToRtpTime(pkt->pts);
	//dbgNotePSL(pkt->pts.time_since_epoch().count()<<','<<tv.tv_sec<<','<<tv.tv_usec<<','<<time);

	uint8* ptr = _buff;
	RTP_HEADER_TIMESTAMP(ptr)	= Swap(time);
	RTP_HEADER_MARKER (ptr) = 0;
	RTP_HEADER_PADDING(ptr) = 0;

	//printf("%llu,%u.%06u,%u,%08x,%08x,%08x,%08x,%s\n",pkt->pts.time_since_epoch().count(),tv.tv_sec,tv.tv_usec,time,time,Swap(time),*((uint32*)ptr+1),((RTPHeader*)ptr)->pts,_smss._codec);
	//printf("%llu,%09llu,%u,%08x,%08x,%08x,%08x,%s\n",pkt->pts.time_since_epoch().count(),pts.count(),time,time,Swap(time),*((uint32*)ptr+1),((RTPHeader*)ptr)->pts,_smss._codec);
}
void RtpSink::buildExtenHeader(uint8* ptr,int idx){
	RTPExternHeader *exhdr = (RTPExternHeader*)ptr;
	exhdr->profile1 = 0;
	exhdr->profile2 = 0;
	exhdr->ehl1 = 0;
	exhdr->ehl0 = (RTP_EXTERN_HEADER_LEN - 4)/4;
	exhdr->index = idx;

}
int RtpSink::UpdateBuff(bool isfirst,int len,int idx){
	uint8* ptr = _buff;
	int hsize=RTP_HEADER_LEN;
	if(isfirst){
		RTP_HEADER_EXTENSION(ptr) = 1;
		buildExtenHeader(ptr+hsize,idx);
		hsize += RTP_EXTERN_HEADER_LEN;
	}else{
		RTP_HEADER_EXTENSION(ptr) = 0;
	}
	hsize += _smss.RtpHeadUpdate(ptr+hsize,len);
	if(len<(_smss._packsize-hsize)) {///last
		RTP_HEADER_MARKER (ptr) = 1;
		//RTP_HEADER_PADDING(ptr) = 1;
		//ptr[_smss._packsize-1]=_smss._packsize-hsize-len;
		//memset(ptr+hsize+len,0,_smss._packsize-hsize-len);
	}

	RTP_HEADER_SEQ (ptr) = Swap(fSeqNo);fSeqNo++;
	return hsize;
}
bool RtpSink::SendBuff(int size){
	//return fRTPInterface.sendPacket(ptr, size);
	int i=0,c=50;
	while(!fRTPInterface.sendPacket(_buff, size) && i++<c){
		putchar('d');
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return i<c;
}

bool RtpSink::Process(Packet* pkt){//dbgTestSL((void*)pkt->data<<','<<pkt->size);
	//dbgPktTraceRecord(*pkt);
	//dbgTestPSL((void*)this<<','<<pkt->pts.time_since_epoch().count());
	uint8*ptr = (uint8*)pkt->data;
	int32 len = pkt->size;
	InitBuff(pkt);//dbgInfoPL();
	_smss.FramePreProcess(ptr,len);
	//dbgLv0(len<<" \t");
	//dbgDump(ptr-5,5);
	bool isfirstpackt = true;
	int frameidx = pkt->index;
	while(len>0){//dbgInfoPSL(len);
		int hsize = UpdateBuff(isfirstpackt,len,frameidx);
		isfirstpackt = false;
		int dsize = _smss._packsize-hsize;
		if(dsize>len) dsize = len;///last		
		memcpy(_buff+hsize,ptr,dsize);
		//returnIfErrC(false,!SendBuff(dsize+hsize));
		if(!SendBuff(dsize+hsize)){
			putchar('P');
			return false;
		}
		len-=dsize;
		ptr+=dsize;
	}
	//dbgPktTraceRecord(*pkt);
	//dbgPktTrace0(*pkt);
	return true;
}

void RtpSink::Loop(){//dbgInfoPL();
	//auto pkt = _src->GeneratePacket();
	auto pkt = _src->GeneratePacket();
	int us = 10000;
	if(pkt && pkt->data && pkt->size) {//dbgTestPSL(_smss._codec);
		Process(pkt.get());		
		us = 1000;
	}
	nextTask() = envir().taskScheduler().scheduleDelayedTask(us, (TaskFunc *) cbLoop, this);

}

void RtpSink::cbLoop(RtpSink*sink){
	//dbgErrPL();
	sink->Loop();
}
uint32 RtpSink::ConvertToRtpTime(const std::chrono::steady_clock::time_point &us) {
	if(_rtpBaseTime==std::chrono::steady_clock::duration::zero()){
		struct timeval tv;
		gettimeofday(&tv, NULL);
		auto tp = std::chrono::seconds(tv.tv_sec)+std::chrono::microseconds(tv.tv_usec);
		_rtpBaseTime = std::chrono::steady_clock::time_point(tp) - us;
	}
	return convertToRTPTimestamp(toTimeVal(us + _rtpBaseTime));
}
