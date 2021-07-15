//
// Created by jg.wang on 2019/7/11.
//

#include "MediaInfo.h"
#include "ITrigger.h"
#pragma once
namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				class IEncoder
					: virtual public common::ITrigger
				{
				public:
					/**
					 * set the codec paramers
					 * @return true/false
					 */
					virtual bool SetCodec(const char*)=0;

					/**
				 * set the target bit rate for codec.
				 * @param br[in] bit per seconds.
				 * @note supported BitRateControl:\n
				 * 		CBRC:\n
				 * 		MBRC:\n
				 * 		DBRC:\n
				 * @see BitRateControl
				 */
					virtual bool SetBitRate(int br)=0;

					/**
					 * is support Shared Encoder.
					 * @return true/false
					 */
					virtual bool IsSupportSharedEncoder() const=0;

					/**
					 * set the group of Shared Encoder.
					 * @param group[in] \n
					 *      0: [default]disable Share Encoder\n
					 * others: the group of Shared Encoder,\n
					 *          using the pipeline of Shared Encoder,\n
					 *          it's name pattern is "plSharedSource[codec][group]"
					 * @return true/false
					 * @note
					 *      if IsSupportSharedEncoder()==false, this api is return false.
					 */
					virtual bool SetSharedEncoderGroup(int group)=0;

					/**
					 * force h264 encoder to output the key frame.
					 */
					virtual void ForceKeyFrame()=0;
				};
			}
		}
	}
}
