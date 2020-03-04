#pragma once
#include "RTPSink.hh"
#include "RtpSource.h"
#include "RtspSmss.h"
#include <chrono>
#include "rtsp.h"
namespace omf{
namespace rtsp {
	class RtpSink
		: public RTPSink {
	public:
		RtpSink(UsageEnvironment &env, Groupsock *RTPgs, RtspSmss &smss, unsigned char rtpPayloadFormat);
		virtual ~RtpSink();
	public:
		virtual Boolean continuePlaying();
		virtual char const *auxSDPLine();
		virtual char const *sdpMediaType() const;
	protected:
		virtual void InitBuff();
		virtual void InitBuff(Packet*);
		virtual void buildExtenHeader(uint8*,int);
		virtual int UpdateBuff(bool isfirst,int len,int idx);
		virtual bool SendBuff(int size);

		//void swapcpy(uint16* dst,uint16* src,int cnt);
	public:
		virtual bool Process(Packet*);

	protected:
		std::chrono::steady_clock::duration _rtpBaseTime;
	public:
		uint32 ConvertToRtpTime(const std::chrono::steady_clock::time_point&);
	public:
		RtspSmss &_smss;
		RtpSource *_src;

		//int _packsize;
		uint8 _buff[1500];

		//CbMemcpy _memcpy;

		std::chrono::steady_clock::duration _clock_diff;
		bool _1st;
	protected:
		void Loop();
		static void cbLoop(RtpSink *);
	};
}
}


