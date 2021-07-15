#pragma once
#include "RtspSmss.h"

NAMESPACE_ICATCH_PLUGIN_BEGIN
namespace rtsp {
	class RtspSmssPcm
		: public RtspSmss
	{
	public:
		RtspSmssPcm(UsageEnvironment*, PacketRing*);
	protected:
		int _width;
		int _depth;
		bool _little;
		bool _signed;
	protected:
		virtual bool Parse();
	};
}
NAMESPACE_ICATCH_PLUGIN_END


