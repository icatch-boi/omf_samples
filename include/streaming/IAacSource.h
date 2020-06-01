//
// Created by jg.wang on 2019/6/28.
//

#pragma once
#include "IAudioSource.h"
#include "common/IEncoder.h"
namespace omf {
	namespace api {
		namespace streaming {
			/**
			 * @example _demoAacSource.cpp
			 * @brief provide YUV date source.
			 * You can get PCM realtime streaming from the interface.\n
			 * Firstly,create an instance.\n
			 * 	CreateNew(const char *keywords).\n
			 * Secondly,set the parameters.\n
			 *  set PCM Device parameters:
			 *  	SelectMIC(int id) \n
			 *  	SetSampleRate(int rate) \n
			 *  	SetChannels(int ch) \n			 *
			 *  set AAC Encoder parameters:\n
			 *  	SetBitRate(int br)\n
			 *  	SetProfile(const char *profile)\n
			 *  	SetTargetChannels(int ch)\n
			 * Thirdly,	register streaming callbck.\n
			 * 	RegisterMessageCallback(const FuncMessage &func)\n
			 * 	RegisterOutputCallback(const IStreamEntry::FuncFrame &func)\n
			 * Finally, operator streaming status.\n
			 * 	Open()\n
			 * 	Close()\n
			 * 	Start()\n
			 * 	Stop()\n
			 * Moreover, get same information from the instance.\n
			 *  GetAacMediaInfo(). call after Open().\n
			 */
			class IAacSource
				: virtual public IAudioSource
				, virtual public common::IEncoder
			{
			public:
				using AacMediaInfo=omf::api::streaming::common::AacMediaInfo;
			public:

				/**
				 * @brief set the profile to the aac codec.
				 * @param profile [in] the profile string.\n
				 * 		"MAIN":Main profile\n
				 * 		"LC" :Low Complexity\n
				 * 		"SSR" :Scalable Sampling Rate profile\n
				 */
				virtual void SetProfile(const char *profile) = 0;

				/**
				 * @brief set the target output channels of aac codec.
				 * @param ch [in] the number of channels.
				 * 		default: 0 ,follow the pcm channels.
				 * 		if(TargetChannels()!=Channels()), this model will run rechannels.
				 */
				virtual void SetTargetChannels(int ch) = 0;

				/**
				 * set bandwidth.
				 * @param bw [0~24000] \n
				 * 		default: 0 ,not limit bandwidth.\n
				 */
				virtual void SetBandWidth(int bw) = 0;
			public:
				/**
				 * get the output aac media info.
				 * @return aac media info.
				 * @see AacMediaInfo
				 */
				virtual common::AacMediaInfo GetAacMediaInfo() const =0;
			public:
				/**
				 * @brief create a new IAacSource instance.
				 * @param keywords[in] the keywords for selecting a IAacSource type.
				 * @return the new IAacSource instance.
				 */
				static IAacSource *CreateNew(const char *keywords);

			protected:
				/**
				 * @note this source is fixed codec:AAC, this api is invalid.
				 * @return false
				 */
				using IAudioSource::SetCodec;
			};
		}
	}
}




