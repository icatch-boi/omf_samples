#pragma once
#include "RtspSmss.h"
namespace omf{
namespace rtsp {
	class RtspSmssH264
		: public RtspSmss
	{
	public:
		RtspSmssH264(UsageEnvironment&, std::shared_ptr<OmfAttrSet>);
	protected:
		uint8* _sps,*_pps;
		int _isps;
		int _ipps;
		uint32 _profile;
		uint32 _level;
		uint32 _gop;
		uint32 _nal;
		bool _rtp1st;

		bool (*_jump)(uint8*&data,int& size);
		//bool jumpDate(uint8*&ptr,int&);
	protected:
		virtual bool Parse()override ;
		//virtual FramedSource *createNewStreamSource(unsigned,unsigned&);
		//virtual RTPSink *createNewRTPSink(Groupsock*,unsigned char,FramedSource*);
		virtual char const *getAuxSDPLine(RTPSink*sink,FramedSource*) override;
		virtual bool FramePreProcess(uint8*&ptr,int& size) override;
		virtual int RtpHeadUpdate(uint8*ptr,int& size)override;

		virtual bool CreateSource()override ;
	};
}
}


