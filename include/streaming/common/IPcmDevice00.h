//
// Created by jg.wang on 2019/7/4.
//
#pragma once
#include "MediaInfo.h"
#include "IAudio.h"

namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				class IPcmDevice :public virtual IAudio{
				public:
					virtual ~IPcmDevice(){}
				public:
					virtual int GetDeviceID() const = 0;
					virtual void GetDeviceID(int devid)=0;
				public:
					/**
					 * @brief whether support stereo or not.
					 * @return true: supported \n
					 * 			false: unsupported \n
					 */
					virtual bool IsSupportStereo() const =0;

				public:
					/**
					 * check whether support volume control or not.
					 * @return supported/unsupported.
					 */
					virtual bool IsSupportVolumeControl() const =0;

					/**
					 * @brief set volume of the device.
					 * @param vol [in] the volume. \n
					 * 		default:50 \n
					 * 		max: 100 \n
					 * 		min: 0 \n
					 */
					virtual void SetVolume(int vol)=0;

					/**
					 * get the volume of the current audio device.
					 * @return volume.
					 */
					virtual int GetVolume()const=0;

					/**
					 * get the volume maximum of the current audio device.
					 * @return maximum supported.
					 */
					virtual int GetVolumeMaximum()const=0;

					/**
					 * get the volume minimum of the current audio device.
					 * @return minimum supported.
					 */
					virtual int GetVolumeMinimum()const=0;
				public:
					/**
					 * check whether support auto gain control(AGC) or not.
					 * @return supported/unsupported.
					 */
					virtual bool IsSupportAutoGainControl() const =0;

					/**
					 * @brief enable the auto gain control(AGC).
					 * 		default:disable
					 */
					virtual void EnableAGC()=0;

					/**
					 * @brief disable the auto gain control(AGC).
					 */
					virtual void DisableAGC()=0;

					/**
					 * get the status of AGC function.
					 * @return true/false\n
					 * 		true: enable \n
					 * 		false: disable or unsupported
					 */
					virtual bool GetAGC()const=0;
				public:
					/**
					 * check whether support mute or not.
					 * @return supported/unsupported.
					 */
					virtual bool IsSupportMute() const =0;

					/**
					 * @brief enable the mute function.
					 * 		default:disalbe
					 */
					virtual void EnableMute()=0;

					/**
					 * @brief disable the mute function.
					 */
					virtual void DisableMute()=0;

					/**
					 * get the status of the mute function.
					 * @return true/false\n
					 *  	true: enable \n
					 * 		false: disable or unsupported
					 */
					virtual bool GetMute()const=0;

				protected:
					static IPcmDevice* _speaker=0;
					static IPcmDevice* _microphone=0;
				public:
					static IPcmDevice* Speaker();
					static IPcmDevice* Microphone();
				};
			}
		}
	}
}