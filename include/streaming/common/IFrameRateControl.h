//
// Created by jg.wang on 2019/7/5.
//
#pragma once

#include "OmfFRCMode.h"
#include <vector>

namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				class IFrameRateControl {
				public:
					virtual std::vector<FRCMode> GetSupportedFRCModes()const = 0;
					virtual bool IsSupportFRCMode(FRCMode)const=0;
					virtual FRCMode GetCurrentFRCMode()const = 0;
					virtual bool SetFRCMode(FRCMode)=0;
				public:
					virtual bool IsSupportFrameRateControl()const = 0;

					/**
					 * get current frame number per seconds.
					 * @return fr.
					 */
					virtual int GetCurrentFrameRate() const = 0;
					/**
					 * set the maximum FR(frame rate).
					 * @param fr [in]the maximum
					 * @note supported BitRateControl:\n
					 * 	 	MFRC:\n
					 * @see BitRateControl
					 */
					virtual bool SetFrameRate(int fr)=0;

				public:
					virtual bool IsSupportFrameRateRange()const = 0;
					/**
					 * set the minimum frame rate.
					 * @param min [in]the minimum
					 * @note supported BitRateControl:\n
					 * 	 	DBRC:\n
					 * 	 	ABRC:\n
					 * @see BitRateControl
					 */
					virtual bool SetFrameRateMinimum(int min)=0;

					/**
					 * set the maximum frame rate.
					 * @param max [in]the maximum
					 * @note supported BitRateControl:\n
					 * 	 	DBRC:\n
					 * 	 	ABRC:\n
					 * @see BitRateControl
					 */
					virtual bool SetFrameRateMaximum(int max)=0;
				public:
					virtual bool SetFluency(int flue)=0;
				public:
					virtual bool IsSupportSingleFrameTrigger() const=0;
					virtual bool Trigger()=0;
				};
			}
		}
	}
}