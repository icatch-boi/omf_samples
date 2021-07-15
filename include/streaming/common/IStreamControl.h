//
// Created by jg.wang on 2019/6/28.
//
#pragma once

#include "_object_base.h"
#include "OmfFrame.h"
#include "OmfMessage.h"
#include "OmfState.h"
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
				class IStreamControl
					:public virtual OmfState
					,public virtual OmfMessage
					,public virtual OmfFrame
					,public virtual object_base
				{
				public:
					/**
					 * set attributes to instance.
					 * @param attributes[in]:the key and value pair string.\n
					 * 		eg.. "width=1920,height=1080,br=2000000,fr=30"
					 * @return
					 */
					virtual bool Set(const char*attributes)=0;
					virtual bool Get(std::string& attributes)=0;
					virtual bool LoadConfig(const char*filename)=0;
					virtual bool FromConfig(const char*params)=0;
				public:
					virtual bool IsSupportedBridgePipeline()const=0;
					virtual bool SetBridgePipeline(const char*plname)=0;
				public:
					virtual bool IsSupportedCache()const=0;
					virtual bool SetCache(int)=0;
				};

				class IStreamSink:public virtual IStreamControl {
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
					virtual bool PushFrame(Frame frm,bool blocking=true)=0;

					/**
					 * register a callback to generate the input frame
					 * @param func the callback funnction
					 * @return true/false
					 */
					virtual bool RegisterInputCallback(const FuncFrameRef &func)=0;
					virtual bool RegisterInputCallback(const FuncFrameReturn &func)=0;

				public:
					/**
					 * get the output media info.
					 * @return the output media infomation string.
					 */
					virtual bool SetMediaInfo(const char*) = 0;
				};

				class IStreamSource:public virtual IStreamControl {
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
					virtual bool PullFrame(Frame& frm,bool blocking=true)=0;
					virtual Frame PullFrame(bool blocking=true)=0;

					/**
					 * register a callback to receive the output frame
					 * @param func the callback funnction
					 * @return true/false
					 */
					virtual bool RegisterOutputCallback(const FuncFrame &func)=0;
					virtual bool RegisterOutputCallback(const FuncFrameRef &func)=0;
				public:
					/**
					 * get the output media info.
					 * @return the output media infomation string.
					 */
					virtual std::string GetMediaInfo()const = 0;

				public:
					/**
					 * is support PreRecord.
					 * @return true/false
					 */
					virtual bool IsSupportPreRecord()const=0;

					/**
					 * set the group of PreRecord.
					 * @param group[in] \n
					 *      0: [default]disable PreRecord\n
					 * others: the group of PreRecord,\n
					 *          using the pipeline of PreRecord,\n
					 *          it's name pattern is "plPreRecord[codec][group]"
					 * @return true/false
					 * @note
					 *      if IsSupportPreRecord()==false, this api is return false.
					 */
					virtual bool SetPreRecordGroup(int group)=0;

				public:
					/**
					 * enable monitor of streaming
					 * @param en
					 * 			0：[default]diasble
					 * 			1: enable
					 * @return true/false
					 */
					virtual bool Monitor(bool en)=0;
				};
			}
		}
	}
}