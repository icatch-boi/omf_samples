//
// Created by jg.wang on 2019/8/22.
//

#pragma once
#include "OmfApiFrame.h"
#include <string>
#include <chrono>
#include <functional>
#include <memory>
#include <vector>
namespace omf {
	namespace webrtc {
		class IYuvSource {
		public:
			using frame_t = omf::api::streaming::common::frame_t;
			using State = omf::api::streaming::common::State;
			using FuncFrame = omf::api::streaming::common::FuncFrame;//std::function<bool(std::shared_ptr<frame_t>&)>;
			using FuncMessage = omf::api::streaming::common::FuncMessage;//std::function<bool(const char *)>;
		public:
			static IYuvSource* CreateNew(const char*);
			virtual ~IYuvSource(){}
		public:
			virtual bool Open(const char*)=0;
			virtual bool Close()=0;
			virtual bool Start()=0;
			virtual bool Stop()=0;
		public:
			virtual bool SendMessage(const char *msg)=0;
			virtual bool RegisterMessageCallback(const FuncMessage&)=0;
		public:
			virtual bool PullFrame(std::shared_ptr<frame_t>&)=0;
			virtual bool RegisterOutputCallback(const FuncFrame&)=0;
		///////////////////////////////////////////////////////////
		public:
			virtual void Set(int w,int h,int fps,const char* fmt)=0;

			virtual int Width()const=0;
			virtual int Height()const=0;
			virtual int FrameRate()const=0;
			virtual const char* Format()const=0;
		public:
			typedef struct{
				int _width;
				int _height;
				int _framerate;
			}capability_t;
			static std::vector<capability_t> GetCapabilities();
		};
	}
}
