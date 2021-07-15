//
// Created by jg.wang on 2019/7/4.
//
#pragma once

namespace omf {
	namespace dev {
		class IAudioDevice	{
		public:
			virtual ~IAudioDevice(){}
		public:
			/**
			 * get the device ID.
			 * @return
			 */
			virtual int DeviceID() const = 0;

			/**
			 * get sample rate.
			 * @return samele rate
			 */
			virtual int Rate() const=0;

			/**
			 * get channels.
			 * @return channels
			 */
			virtual int Channels() const=0;

			virtual int Depth() const =0;

			virtual int Width() const =0;
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
			virtual bool Volume(int vol)=0;

			/**
			 * get the volume of the current audio device.
			 * @return volume.
			 */
			virtual int Volume()const=0;

			/**
			 * get the volume maximum of the current audio device.
			 * @return maximum supported.
			 */
			virtual int VolumeMax()const=0;

			/**
			 * get the volume minimum of the current audio device.
			 * @return minimum supported.
			 */
			virtual int VolumeMin()const=0;
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
			virtual bool AGC(bool en)=0;

			/**
			 * get the status of AGC function.
			 * @return true/false\n
			 * 		true: enable \n
			 * 		false: disable or unsupported
			 */
			virtual bool AGC()const=0;
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
			virtual bool Mute(bool en)=0;

			/**
			 * get the status of the mute function.
			 * @return true/false\n
			 *  	true: enable \n
			 * 		false: disable or unsupported
			 */
			virtual bool Mute()const=0;
		public:
			/**
			 * check whether support gain or not.
			 * @return supported/unsupported.
			 */
			virtual bool IsSupportGain()const=0;

			/**
			 * @brief enable the gain function.
			 * 		default:disalbe
			 */
			virtual bool Gain(int gain)=0;

			/**
			 * get the status of the gain function.
			 * @return true/false\n
			 *  	true: enable \n
			 * 		false: disable or unsupported
			 */
			virtual int Gain()const=0;

			/**
			 * get the gain maximum of the current audio device.
			 * @return maximum supported.
			 */
			virtual int GainMax()const=0;

			/**
			 * get the Gain minimum of the current audio device.
			 * @return minimum supported.
			 */
			virtual int GainMin()const=0;
		public:
			static omf::dev::IAudioDevice* CreateNow(const char*type00,const char*keywords);
			static omf::dev::IAudioDevice* Speaker(const char*keywords=0);
			static omf::dev::IAudioDevice* Microphone(const char*keywords=0);
		};

	}
}