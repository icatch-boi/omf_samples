//
// Created by jg.wang on 2019/8/22.
//

#pragma once
#include "OmfApiFrame.h"
#include <string>
#include <chrono>
#include <functional>
#include <memory>
namespace omf {
	namespace webrtc {
		class IPcmPlayer {
		public:
			using frame_t = omf::api::streaming::common::frame_t;
			using State = omf::api::streaming::common::State;
			using FuncFrame = omf::api::streaming::common::FuncFrame;//std::function<bool(std::shared_ptr<frame_t>&)>;
			using FuncMessage = omf::api::streaming::common::FuncMessage;//std::function<bool(const char *)>;
		public:
			static IPcmPlayer* CreateNew(const char*);
			virtual ~IPcmPlayer(){}
		public:
			virtual bool Open(const char*)=0;
			virtual bool Close()=0;
			virtual bool Start()=0;
			virtual bool Stop()=0;
		public:
			virtual bool SendMessage(const char *msg)=0;
			virtual bool RegisterMessageCallback(const std::function<bool(const char*)> &func)=0;
		public:
			virtual bool PushFrame(std::shared_ptr<frame_t>&)=0;
			virtual bool RegisterInputCallback(std::function<bool(std::shared_ptr<frame_t>&)>)=0;
		///////////////////////////////////////////////////////////
		public:
			virtual int Channels()const=0;
			virtual int Rate()const=0;
			virtual int ByteWidth()const=0;
		public:
			virtual bool CanVolume()const=0;
			virtual int Volume()const=0;
			virtual int VolumeMax()const=0;
			virtual int VolumeMin()const=0;
			virtual bool Volume(int v)=0;
		public:
			virtual bool CanAgc()const=0;
			virtual bool Agc()const=0;
			virtual bool Agc(bool v)=0;
		public:
			virtual bool CanMute()const=0;
			virtual bool Mute()const=0;
			virtual bool Mute(bool v)=0;
		};
	}
}


