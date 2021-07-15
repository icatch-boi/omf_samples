#pragma once
#include "RtspSmss.h"

namespace omf{
namespace rtsp {
	class RtspSmssAac
		: public RtspSmss
	{
	public:
		RtspSmssAac(UsageEnvironment&, std::shared_ptr<OmfAttrSet>);
	protected:

		bool _rtp1st;

		int _version;
		int _profile;
		int _rateidx;
		int _blocknum;
		//bool _hasadts;
		//uint8* _adts;
		int _adtslen;
	protected:
		virtual bool Parse();
		//virtual FramedSource *createNewStreamSource(unsigned,unsigned&);
		//virtual RTPSink *createNewRTPSink(Groupsock*,unsigned char,FramedSource*);
		virtual char const *getAuxSDPLine(RTPSink*sink,FramedSource*) override ;
		virtual bool FramePreProcess(uint8*&ptr,int& size) override;
		virtual int RtpHeadUpdate(uint8*ptr,int& size) override;

		virtual bool CreateSource()override ;
	};
}
}


