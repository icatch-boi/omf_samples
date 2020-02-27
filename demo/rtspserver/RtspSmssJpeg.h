#pragma once
#include "RtspSmss.h"

NAMESPACE_ICATCH_PLUGIN_BEGIN
namespace rtsp {
	class RtspSmssJpeg
		: public RtspSmss
	{
	public:
		RtspSmssJpeg(UsageEnvironment*, PacketRing*);
	protected:
		int32 _width;//=0;
		int32 _height;//=0;

		int8 _qtableIndex;//=0;
		int8 _qtablePrecision;//=0;
		int32 _qtableOffset;//=0;
		int32 _qtableLen;//=0;
		int32 _qtableCnt;//=0;
		int32 _vlcoffset;//=0;
		uint8 _sampleType;//=0;//0:422 1:420

		uint8* _qtablePtr;//=0;
		bool _parsed;// = false;
		int32 _index;

		object* (*_parser)(uint8*,int);
	protected:
		virtual bool Parse()OVERRIDE;
		virtual bool FramePreProcess(uint8*&ptr,int& size) OVERRIDE;
		virtual int RtpHeadUpdate(uint8*ptr,int& size)OVERRIDE;

	};
}
NAMESPACE_ICATCH_PLUGIN_END


