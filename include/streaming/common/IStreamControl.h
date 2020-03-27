//
// Created by jg.wang on 2019/6/28.
//
#pragma once

#include "_object_base.h"
#include "OmfApiCommon.h"
#include <string>
#include <functional>
#include <chrono>
#include <memory>
namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				/**
				 * the base operator interface of a streaming
				 */
				class IStreamControl {
				public:
					const std::type_info& GetType() const{return typeid(*this);}
				public:
					using frame_t = omf::api::streaming::common::frame_t;
					using State = omf::api::streaming::common::State;
					using FuncFrame = omf::api::streaming::common::FuncFrame;
					using FuncMessage = omf::api::streaming::common::FuncMessage;
				public:
					/**
					 * set attributes to instance.
					 * @param attributes[in]:the key and value pair string.\n
					 * 		eg.. "width=1920,height=1080,br=2000000,fr=30"
					 * @return
					 */
					virtual bool Set(const char*attributes)=0;
				public:
					/**
					 * change up state, ready/play. \n
					 * 	\li	ready:\n
					 * 	\li	play:\n
					 * @return true/false
					 */
					virtual bool ChangeUp(State)=0;
					/**
					 * change down state, ready/null. \n
					 * 	\li	ready:\n
					 * 	\li	play:\n
					 * @return
					 */
					virtual bool ChangeDown(State)=0;
					/**
					 * change state, up or down, null/ready/plsy. \n
					 * 	\li	null:\n
					 * 	\li	ready:\n
					 * 	\li	play:\n
					 * @return
					 */
					virtual bool ChangeState(State)=0;
					/**
					 * get current state.
					 * @return state:\n
					 * 	\li null:\n
					 * 	\li ready:\n
					 * 	\li play:\n
					 */
					virtual State CurrentState()const=0;
				public:
					/**
					 * register a callback to process the current streaming message.
					 * @param func the callback funnction
					 */
					virtual void RegisterMessageCallback(const FuncMessage &func)=0;

					/**
					 * send a message to current streaming.
					 * @param msg the message script string.
					 * 		eg.. type=message,name=xxx,...
					 * @return true/false
					 */
					virtual bool SendMessage(const char *msg)=0;

				};

				class IStreamInput:public virtual IStreamControl {
				public:
					/**
					 * check whether the push-operation is supported.
					 * @return true/false
					 */
					virtual bool IsSupportedPushFrame()const =0;
					/**
					 * check whether the register-input-callback operation is supported.
					 * @return true/false
					 */
					virtual bool IsSupportedInputFrameCallback()const = 0;
				public:
					/**
					 * push a frame to current streaming
					 * @param frm [in]a streaming frame
					 * @return true/false
					 */
					virtual bool PushFrame(std::shared_ptr<frame_t> frm)=0;

					/**
					 * register a callback to generate the input frame
					 * @param func the callback funnction
					 * @return true/false
					 */
					virtual bool RegisterInputCallback(const FuncFrame &func)=0;
				};

				class IStreamOutput:public virtual IStreamControl {
				public:
					/**
					 * check whether the pull-operation is supported.
					 * @return true/false
					 */
					virtual bool IsSupportedPullFrame()const =0;
					/**
					 * check whether the register-output-callback operation is supported.
					 * @return true/false
					 */
					virtual bool IsSupportedOutputFrameCallback()const = 0;
				public:
					/**
					 * pull a frame from current streaming
					 * @param frm [out]a streaming frame
					 * @return true/false
					 */
					virtual bool PullFrame(std::shared_ptr<frame_t>& frm,bool blocking=true)=0;
					virtual std::shared_ptr<frame_t> PullFrame(bool blocking=true)=0;

					/**
					 * register a callback to receive the output frame
					 * @param func the callback funnction
					 * @return true/false
					 */
					virtual bool RegisterOutputCallback(const FuncFrame &func)=0;
				public:
					/**
					 * get the output media info.
					 * @return the output media infomation string.
					 */
					virtual std::string GetMediaInfo()const = 0;
				};
			}
		}
	}
}