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
			 *@example _demoPcmPlayer.cpp
			 */
			/**
			 * @brief provide PCM Player.
			 * You can play PCM streaming from the interface.\n
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
			 *  GetPcmMediaInfo(). call after Open().\n			 *
			 */
			class IPcmPlayer
					: virtual public common::IStreamSink
					, virtual public common::IAudio
			{
			public:
				using PcmMediaInfo=omf::api::streaming::common::PcmMediaInfo;
				using ISpeaker=omf::dev::IAudioDevice;
			public:
				/**
				 * select the speaker device.
				 * @param keywords [in] the speaker device keywords. \n
		 		 *		default: 0 select current speaker.\n
				 */
				virtual bool SelectSpeaker(const char*keywords=0)=0;

				/**
				 * get the selected speaker;
				 * @return the selected speaker pointer
				 */
				virtual ISpeaker* Speaker() = 0;
			public:
				/**
				 * get the output pcm media info.
				 * @return pcm media info.
				 * @see PcmMediaInfo
				 */
				virtual PcmMediaInfo GetPcmMediaInfo() const =0;
			public:///createNew
				/**
				 * @brief create a new IPcmPlayer instance.
				 * @param keywords[in] the keywords for selecting a IPcmPlayer type.
				 * @return the new IPcmSource instance.
				 */
				static IPcmPlayer *CreateNew(const char *keywords);
				static IPcmPlayer* CreateNewFromConfig(const char *config);
				static IPcmPlayer* CreateNewFromFile(const char *file);
			};
		}
	}
}

