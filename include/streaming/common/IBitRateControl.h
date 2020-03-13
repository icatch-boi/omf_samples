//
// Created by jg.wang on 2019/7/5.
//
#pragma once

#include "IEncoder.h"
#include <vector>

namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				class IBitRateControl:public virtual IEncoder {
				public:
					virtual ~IBitRateControl(){}
				public:
					typedef struct{const char* mode;const char* note;}mode_t;
					virtual bool SetBrcMode(const char*)=0;
					virtual std::vector<mode_t> GetBrcModes()const = 0;
					virtual const mode_t& GetBrcMode()const = 0;
				public:
					virtual bool IsSupportFrameRateControl()const = 0;
					virtual bool IsSupportFrameRateRange()const = 0;

					/**
					 * set the maximum FR(frame rate).
					 * @param fr [in]the maximum
					 * @note supported BitRateControl:\n
					 * 	 	MFRC:\n
					 * @see BitRateControl
					 */
					virtual bool SetFrameRate(int fr)=0;

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
					virtual bool IsSupportBitRateControl()const = 0;
					virtual bool IsSupportBitRateRange()const = 0;

					/**
					 * set the minimum bit rate(BR).
					 * @param min [in]the minimum
					 * @note supported BitRateControl:\n
					 * 	 	ABRC:\n
					 * @see BitRateControl
					 */
					virtual bool SetBitRateMinimum(int min)=0;

					/**
					 * set the maximum bit rate(BR).
					 * @param max [in]the maximum
					 * @note supported BitRateControl:\n
					 * 	 	ABRC:\n
					 * @see BitRateControl
					 */
					virtual bool SetBitRateMaximum(int max)=0;

				public:
					virtual bool IsSupportQPControl()const = 0;
					virtual bool IsSupportQPRange()const = 0;

					/**
					 * manual set the quantization parameter.
					 * @param qp [in] quantization parameter. \n
					 * 		min: 10 \n
					 * 		max: 50 \n
					 * @note supported BitRateControl:\n
					 * 	 	MQPC: \n
					 * @see BitRateControl
					 */
					virtual bool SetQP(int qp)=0;

					/**
					 * set the minimum quantization parameter(QP).
					 * @param min [in]the minimum
					 * @note supported BitRateControl:\n
					 * 	 	MBRC:\n
					 * 	 	DBRC:\n
					 * 	 	ABRC:\n
					 * @see BitRateControl
					 */
					virtual bool SetQPMinimum(int min)=0;

					/**
					 * set the maximum quantization parameter(QP).
					 * @param max [in]the maximum
					 * @note supported BitRateControl:\n
					 * 	 	MBRC:\n
					 * 	 	DBRC:\n
					 * 	 	ABRC:\n
					 * @see BitRateControl
					 */
					virtual bool SetQPMaximum(int max)=0;

				public:
					/**
					 * get current frame number per seconds.
					 * @return fr.
					 */
					virtual int GetCurrentFrameRate() const = 0;

					/**
					 * get current bit per seconds.
					 * @return br.
					 */
					virtual int GetCurrentBitRate() const = 0;

					/**
					 * get current quantization parameter(QP).
					 * @return QP.
					 */
					virtual int GetCurrentQP() const = 0;
				};
			}
		}
	}
}