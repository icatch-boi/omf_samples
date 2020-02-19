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
				class IEncoder {
				public:
					/**
				 * set the target bit rate for codec.
				 * @param br[in] bit per seconds.
				 * @note supported BitRateControl:\n
				 * 		CBRC:\n
				 * 		MBRC:\n
				 * 		DBRC:\n
				 * @see BitRateControl
				 */
					virtual void SetBitRate(int br)=0;
				};
			}
		}
	}
}
