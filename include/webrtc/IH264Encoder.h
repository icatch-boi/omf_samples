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
		class IH264Encoder {
		public:
			using frame_t = omf::api::streaming::common::frame_t;
			using State = omf::api::streaming::common::State;
			using FuncFrame = omf::api::streaming::common::FuncFrame;
			using FuncMessage = omf::api::streaming::common::FuncMessage;
		public:
			static IH264Encoder* CreateNew(const char*);
			virtual ~IH264Encoder(){}
		public:
			virtual bool Open(const char*)=0;
			virtual bool Close()=0;
			virtual bool Start()=0;
			virtual bool Stop()=0;
		public:
			virtual bool SendMessage(const char *msg)=0;
			virtual bool RegisterMessageCallback(const FuncMessage&)=0;
		public:
			virtual bool Trigger(std::shared_ptr<frame_t>&,bool forcekeyframe=false)=0;
			virtual bool RegisterOutputCallback(const FuncFrame&)=0;
			virtual bool Encode(std::shared_ptr<frame_t>&,std::shared_ptr<frame_t>&,bool forcekeyframe=false)=0;
		///////////////////////////////////////////////////////////
		public:
			virtual bool Set(const char*)=0;
		public:
			virtual bool Set(int w,int h,int fr,const char* yuvformat)=0;
			virtual bool Set(const char*goptype,int gop)=0;
			virtual bool Set(int br,int fr)=0;
		public:
			typedef struct{
				int _spsOffset;
				int _spsLenght;
				int _ppsOffset;
				int _ppsLenght;
				int _dataOffset;
			}frame_struct_t;
			virtual frame_struct_t GetIFrameStruct()const=0;
			virtual frame_struct_t GetPFrameStruct()const=0;
			typedef struct{
				char* data;
				int size;
			}nal_t;
			virtual nal_t GetExtra()const=0;
			virtual nal_t GetSPS()const=0;
			virtual nal_t GetPPS()const=0;

			virtual int GetQp()const=0;
			virtual int GetBr()const=0;
			virtual int GetFr()const=0;

			virtual int GetWidth()const=0;
			virtual int GetHeight()const=0;
		};
	}
}
