//
// Created by jg.wang on 2019/6/28.
//

#pragma once

#include "IAudioSource.h"
namespace omf {
	namespace api {
		namespace streaming {
			/**
			 * @example _demoPcmSource.cpp
			 * @brief provide YUV date source.
			 * You can get PCM realtime streaming from the interface.\n
			 * Firstly,create an instance.\n
			 * 	CreateNew(const char *keywords).\n
			 * Secondly,set the parameters.\n
			 *  SetSampleRate(int rate) \n
			 *  SetChannels(int ch) \n
			 * Thirdly,	register streaming callbck.\n
			 * 	RegisterMessageCallback(const FuncMessage &func)\n
			 * 	RegisterOutputCallback(const IStreamControl::FuncFrame &func) \n
			 * Finally, operator streaming status.\n
			 * 	Open()\n
			 * 	Close()\n
			 * 	Start()\n
			 * 	Stop()\n
			 * Moreover, get same information from the instance.\n
			 *  GetPcmMediaInfo(). call after Open().\n
			 */
			class IPcmSource
				: virtual public IAudioSource
			{
			public:
				using PcmMediaInfo=omf::api::streaming::common::PcmMediaInfo;
			public:
				/**
				 * get the output pcm media info.
				 * @return pcm media info.
				 * @see PcmMediaInfo
				 */
				virtual PcmMediaInfo GetPcmMediaInfo() const =0;
			public:
				/**
				 * @brief create a new IPcmSource instance.
				 * @param keywords[in] the keywords for selecting a IPcmSource device.
				 * @return the new IPcmSource instance.
				 */
				static IPcmSource *CreateNew(const char *keywords);
				static IPcmSource* CreateNewFromConfig(const char *config);
				static IPcmSource* CreateNewFromFile(const char *file);
			public:
				/**
				 * get the IPcmSource capabilities list.
				 * @param keywords [in] the keywords for selecting a IPcmSource device.
				 * @return the capabilities list.
				 */
				static std::vector<PcmMediaInfo> GetCapabilities(const char*keywords);
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
