//
// Created by jg.wang on 2019/6/28.
//

#pragma once

#include "IAudioDevice.h"
#include "common/IAudio.h"
#include "common/MediaInfo.h"
#include "common/IStreamControl.h"
#include <vector>
namespace omf {
	namespace api {
		namespace streaming {
			/**
			 * @brief provide audio date source.
			 * You can get AUDIO realtime streaming from the interface.\n
			 * Firstly,create an instance.\n
			 * 	CreateNew(const char *keywords).\n
			 * Secondly,set the parameters.\n
			 *  SelectMIC(int id) \n
			 *  SetSampleRate(int rate) \n
			 *  IsSupportStereo()\n
			 *  	SetChannels(int ch) \n
			 *  IsSupportVolumeControl()\n
			 *  	SetVolume(int vol)\n
			 *  IsSupportAutoGainControl()\n
			 *  	EnableAGC()\n
			 *  	DisableAGC()\n
			 *  IsSupportMute()\n
			 *  	EnableMute()\n
			 *  	DisableMute()\n
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
				: virtual public common::IStreamOutput
				, virtual public common::IAudio
			{
			public:
				using AudioMediaInfo=omf::api::streaming::common::AudioMediaInfo;
				using IMicrophone=omf::dev::IAudioDevice;
			public:
				/**
				 * select the MIC device.
				 * @param keywords [in] the microphone device keywords. \n
		 		 *		default: 0 select current microphone \n
				 */
				virtual bool SelectMicrophone(const char*keywords=0)=0;

				/**
				 * get the selected microphone;
				 * @return the selected Microphone pointer
				 */
				virtual IMicrophone* Microphone() = 0;

				/**
				 * set the codec paramers
				 * @return true/false
				 */
				virtual bool SetCodec(const char*)=0;
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
