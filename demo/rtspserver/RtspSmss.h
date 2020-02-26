#pragma once
#include "OnDemandServerMediaSubsession.hh"
#include "rtsp.h"
namespace omf{
namespace rtsp{
	class RtspSmss
		:public OnDemandServerMediaSubsession
	{
	public:
		RtspSmss(UsageEnvironment&,std::shared_ptr<OmfAttrSet>);
		virtual ~RtspSmss();

	protected:
		bool init(const char* type
			,const char* codec
			,int rate
			,int channels
			);
	protected: // redefined virtual functions
		virtual FramedSource *createNewStreamSource(unsigned,unsigned&);
		virtual RTPSink *createNewRTPSink(Groupsock*,unsigned char,FramedSource*);
		virtual bool Parse() = 0;
	public:
		std::shared_ptr<OmfAttrSet> _attr=0;
		ISource*_src=0;
		//int _br=0;
		//int _offset=0;
		//int _framemax=0;
		//CbMemcpy _copy=0;
		int _packsize=0;
		int _payloadtype=0;
		int _rate=0;
		int _channels=0;
		int _headsize=0;
		char const* _type=0;
		char const* _codec=0;
		char* _auxsdp=0;
	public:
		virtual bool FramePreProcess(uint8*&ptr,int& size);
		virtual int RtpHeadUpdate(uint8*ptr,int& size);
		virtual char const *getAuxSDPLine(RTPSink*sink,FramedSource*);
		//virtual const char* AuxSdp(uint8 payloadtype);
		virtual bool CreateSource()=0;
	};
}
}


