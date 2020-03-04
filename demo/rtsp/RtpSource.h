#pragma once

#include "UsageEnvironment.hh"
#include "FramedSource.hh"
#include "rtsp.h"
namespace omf{
namespace rtsp {
	class RtpSource
		: public FramedSource {
	public:
		RtpSource(UsageEnvironment &env, ISource* reader);
		virtual ~RtpSource();
	protected:
		virtual void doGetNextFrame();
		virtual bool EOS();
	protected:
		ISource* _reader;
		int _us;
		bool _1st;
	public:
		virtual std::shared_ptr<Packet> GeneratePacket();
		int DelayTimeUs() const;
	};
}
}
