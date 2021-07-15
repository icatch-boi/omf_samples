//
// Created by jg.wang on 2019/7/11.
//
#pragma once
#include "IBitRateControl.h"
namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				class IEncoder:public virtual IBitRateControl
				{
				public:
					/**
					 * set the codec paramers
					 * @return true/false
					 */
					virtual bool SetCodec(const char*)=0;

					/**
					 * is support PreRecord.
					 * @return true/false
					 */
					virtual bool IsSupportPreRecord()const=0;

					/**
					 * set the group of PreRecord.
					 * @param group[in] \n
					 *      0: [default]disable PreRecord\n
					 * others: the group of PreRecord,\n
					 *          using the pipeline of PreRecord,\n
					 *          it's name pattern is "plPreRecord[codec][group]"
					 * @return true/false
					 * @note
					 *      if IsSupportPreRecord()==false, this api is return false.
					 */
					virtual bool SetPreRecordGroup(int group)=0;

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

					virtual bool IsSupportForceKeyFrame()const=0;
					/**
					 * force h264 encoder to output the key frame.
					 */
					virtual bool ForceKeyFrame()=0;
				};
			}
		}
	}
}
