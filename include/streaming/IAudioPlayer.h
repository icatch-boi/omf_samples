//
// Created by jg.wang on 2019/6/28.
//

#pragma once

#include "IAudioDevice.h"
#include "common/IAudio.h"
#include "common/MediaInfo.h"
#include "common/IStreamControl.h"
namespace omf{
	namespace api {
		namespace streaming {
			/**
			 *@example _demoAudioPlayer.cpp
			 */
			/**
			 * @brief provide Audio Player.
			 * You can play audio streaming from the interface.\n
			 * Firstly,create an instance.\n
			 * 	CreateNew(const char *keywords).\n
			 * Secondly,set the parameters.\n
			 *  SelectSpeaker(int id) \n
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
			 * Finally, operator streaming status.\n
			 * 	Open()\n
			 *  Start()\n
			 * 	PushFrame(std::shared_ptr<IStreamControl::frame_t> frm).\n
			 * 	Stop()\n
			 * 	Close()\n
			 * Moreover, get same information from the instance.\n
			 *  GetAudioMediaInfo(). call after Open().\n			 *
			 */
			class IAudioPlayer
					: virtual public common::IStreamSink
					, virtual public common::IAudio
			{
			public:
				using AudioMediaInfo=omf::api::streaming::common::AudioMediaInfo;
				using ISpeaker=omf::dev::IAudioDevice;
			public:
				/**
				 * select the speaker device.
				 * @param keywords [in] the speaker device keywords. \n
		 		 *		default: 0 select current speaker.\n
		 		 * @note [Expired API][Invalid]
				 */
				virtual bool SelectSpeaker(const char*keywords=0)=0;

				/**
				 * get the selected speaker;
				 * @return the selected speaker pointer
				 */
				virtual ISpeaker* Speaker() = 0;

				/**
				 * set the codec paramers
				 * @return true/false
				 */
				//virtual bool SetCodec(const char*)=0;
			public:
				/**
				 * get the output pcm media info.
				 * @return pcm media info.
				 * @see PcmMediaInfo
				 */
				virtual AudioMediaInfo GetAudioMediaInfo() const =0;
			public:///createNew
				/**
				 * @brief create a new IPcmPlayer instance.
				 * @param keywords[in] the keywords for selecting a IPcmPlayer type.
				 * @return the new IPcmSource instance.
				 */
				static IAudioPlayer *CreateNew(const char *);
			};
		}
	}
}

