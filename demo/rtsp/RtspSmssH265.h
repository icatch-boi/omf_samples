#pragma once
#include "RtspSmss.h"

NAMESPACE_ICATCH_PLUGIN_BEGIN
namespace rtsp {
	class RtspSmssH265
		: public RtspSmss
	{
	public:
		RtspSmssH265(UsageEnvironment*, PacketRing*);
	protected:
		uint8* _vps,*_sps,*_pps;
		int _ivps;
		int _isps;
		int _ipps;
		uint32 _profile;
		uint32 _level;
		uint32 _gop;
		uint32 _nal;
		bool _rtp1st;
	protected:
		virtual bool Parse()OVERRIDE;
		//virtual FramedSource *createNewStreamSource(unsigned,unsigned&);
		//virtual RTPSink *createNewRTPSink(Groupsock*,unsigned char,FramedSource*);
		virtual char const *getAuxSDPLine(RTPSink*sink,FramedSource*) OVERRIDE;
		virtual bool FramePreProcess(uint8*&ptr,int& size) OVERRIDE;
		virtual int RtpHeadUpdate(uint8*ptr,int& size)OVERRIDE;
	};
}
NAMESPACE_ICATCH_PLUGIN_END


