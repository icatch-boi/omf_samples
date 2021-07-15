//
// Created by jg.wang on 2019/6/28.
//

#pragma once

#include "IAudioDevice.h"
#include "common/IAudio.h"
#include "common/MediaInfo.h"
#include "common/IStreamControl.h"
#include "common/IEncoder.h"
#include <vector>
namespace omf {
	namespace api {
		namespace streaming {
			/**
			 * @example _demoAudSrc.cpp
			 * @brief provide audio date source.
			 * You can get AUDIO realtime streaming from the interface.\n
			 * Firstly,create an instance.\n
			 * 	CreateNew(const char *keywords).\n
			 * Secondly,set the parameters.\n
			 *  SelectMIC(int id) \n
			 *  SetSampleRate(int rate) \n
			 *  SetChannels(int ch) \n
			 *  SetCodec(const char* codec)\n
			 * Thirdly,	register streaming callbck.\n
			 * 	RegisterMessageCallback(const FuncMessage &func)\n
			 * 	RegisterOutputCallback(const IStreamControl::FuncFrame &func) \n
			 * Finally, operator streaming status.\n
			 * 	Open()\n
			 * 	Close()\n
			 * 	Start()\n
			 * 	Stop()\n
			 * Moreover, get same information from the instance.\n
			 *  GetAudioMediaInfo(). call after Open().\n
			 */
			class IAudioSource
				: virtual public common::IStreamSource
				, virtual public common::IAudio
				, virtual public common::IEncoder
			{
			public:
				using AudioMediaInfo=omf::api::streaming::common::AudioMediaInfo;
				using IMicrophone=omf::dev::IAudioDevice;
			public:
				/**
				 * select the MIC device.
				 * @param keywords [in] the microphone device keywords. \n
		 		 *		default: 0 select current microphone \n
		 		 * @note The default MIC of the system is used, which is usually not set.
				 */
				virtual bool SelectMicrophone(const char*keywords=0)=0;

				/**
				 * get the selected microphone;
				 * @return the selected Microphone pointer
				 */
				virtual IMicrophone* Microphone() = 0;

			public:
				/**
				 * check if support AEC(acoustic -echo-cancel).
				 * @return true/false
				 */
				virtual bool IsSupportAEC()const=0;
				/**
				 * enable/disable AEC and set param
				 * @param enable [in]enable/disable
				 * @param param [in]params string
				 * @return true/false
				 */
				virtual bool SetAEC(bool enable,const char*param)=0;

			public:
				/**
				 * check if support ANS(acoustic-noise-suppress).
				 * @return true/false
				 */
				virtual bool IsSupportANS()const=0;
				/**
				 * enable/disable ANS and set param
				 * @param enable [in]enable/disable
				 * @param param [in]params string
				 * @return true/false
				 */
				virtual bool SetANS(bool enable,const char*param)=0;
			public:
				/**
				 * get the output pcm media info.
				 * @return pcm media info.
				 * @see PcmMediaInfo
				 */
				virtual AudioMediaInfo GetAudioMediaInfo() const =0;
			public:
				/**
				 * @brief create a new IPcmSource instance.
				 * @param keywords[in] the keywords for selecting a IPcmSource device.
				 * @return the new IPcmSource instance.
				 */
				static IAudioSource *CreateNew(const char *keywords);
				static IAudioSource* CreateNewFromConfig(const char *config);
				static IAudioSource* CreateNewFromFile(const char *file);
			public:
				/**
 * get the IPcmSource capabilities list.
 * @param keywords [in] the keywords for selecting a IPcmSource device.
 * @return the capabilities list.
 */
				static std::vector<AudioMediaInfo> GetCapabilities(const char*keywords);

			};
		}
	}
}
