//
// Created by jg.wang on 2019/7/11.
//

#pragma once
namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				class IAudio {
				public:
					virtual ~IAudio(){}
				public:
					/**
					 * set sample rate.
					 * @param rate [in]the number of sample rate.\n
					 * 		default: 48000 \n
					 * 		region: 48000,44100,32000,16000,8000 \n
					 */
					virtual void SetSampleRate(int rate)=0;

					/**
					 * set the channels.
					 * @param ch [in]the number of channels. \n
					 * 		default: 2 \n
					 * 		region: 1,2 \n
					 */
					virtual void SetChannels(int ch)=0;
				};
			}
		}
	}
}
