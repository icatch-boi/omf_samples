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
					enum class BitRateControl {
						nothing = 0,
						CBRC,/**< Constant Bit Rate Control. \n
 								 * Adjust bit stream size to follow the target bit-rate. \n
 								 * The given parameter[br] takes effect at next GOP.\n
 								 * Not affect frame rate control.\n
 								 * @li SetQP(int qp) is invalid. \n
								 * @li SetBitRate(int br) is valid and takes effect at next GOP. \n
								 * @li SetBitRateMinimum(int min) is invalid.\n
				 				 * @li SetBitRateMaximum(int max) is invalid.\n
								 */
						MBRC,    /**< Manual Bit Rate Control. \n
 								 * Adjust streaming bit-rate to follow the target bit-rate.\n
 								 * The given parameter[br] takes effect at next FRAME.\n
 								 * Not affect frame rate control.
  								 * @li SetQP(int qp) is invalid. \n
								 * @li SetBitRate(int br) is valid and takes effect every frame. \n
								 * @li SetBitRateMinimum(int min) is invalid.\n
				 				 * @li SetBitRateMaximum(int max) is invalid.\n
								 */
						DBRC,    /**< Dynamic Bit Rate Control. \n
								 * Adjust streaming bit-rate to follow the target bit-rate.\n
 								 * The given parameter[br] takes effect at next FRAME.\n
 								 * Frame-rate is calculated automatically to achieve the purpose of regulating the bit stream.
								 * @li SetQP(int qp) is invalid. \n
								 * @li SetBitRate(int br) is valid and takes effect every frame. \n
								 * @li SetBitRateMinimum(int min) is invalid.\n
								 * @li SetBitRateMaximum(int max) is invalid.\n
								 * @li SetFrameRate(int fr) is invalid and the fr is calculated by algorithm. \n
								 * @li SetFrameRateMinimum(int min) is valid. \n
								 * @li SetFrameRateMaximum(int max) is valid. \n
								 * */
						ABRC,    /**< Self-Adapt Bit Rate Control. \n
 								 * According to pipeline congestion,\n
 								 * automatically adjust bit stream size
 								 * to adapt to pipeline bandwidth.\n
 								 * frame-rate and bit-rate are calculated automatically.\n
								 * @li SetQP(int qp) is invalid. \n
								 * @li SetBitRate(int br) is invalid.\n
								 * @li SetFrameRate(int fr) is invalid.\n
								 * @li SetFrameRateMinimum(int min) is valid.\n
								 * @li SetFrameRateMaximum(int max) is valid.\n
								 * @li SetBitRateMinimum(int min) is valid.\n
								 * @li SetBitRateMaximum(int max) is valid.\n
								 */
						MFRC,    /**< Manual Frame Rate Control. \n
 								 * Adjust streaming frame-rate to follow the target.\n
 								 * Not affect bit rate control.\n
								 * @li SetFrameRate(int fr) is valid. \n
								 * @li SetFrameRateMinimum(int min) is invalid. \n
								 * @li SetFrameRateMaximum(int max) is invalid. \n
								 */
						MQPC,    /**< Manual Quantization Parameter Control.\n
 								 * Provide parameter[QP] directly to codec.
 								 * Not affect frame rate control.\n
								 * @li SetQP(int qp) is valid and takes effect every frame. \n
								 * @li SetBitRate(int br) is invalid. \n
								 * @li SetBitRateMinimum(int min) is invalid.\n
				 				 * @li SetBitRateMaximum(int max) is invalid.\n
								 */
					};

				public:
					/**
					 * get the device supported BRC list.
					 * @return the BitRateControl list
					 * @see BitRateControl
					 */
					virtual std::vector <BitRateControl> BitRateControlSupportedList() const =0;

					/**
					 * check whether support the spec BRC mode or not.\n
					 * @param brc [in] the mode
					 * @return supported/unsupported.
					 * @see BitRateControl
					 */
					virtual bool IsSupport(BitRateControl brc) const =0;

					/**
					 * select the bit rate control(BRC).
					 * @param brc [in] BitRateControl
					 * @return true/false
					 * @see BitRateControl
					 */
					virtual bool SelectBitRateControl(BitRateControl brc) =0;

				public:
					/**
					 * set the maximum FR(frame rate).
					 * @param fr [in]the maximum
					 * @note supported BitRateControl:\n
					 * 	 	MFRC:\n
					 * @see BitRateControl
					 */
					virtual void SetFrameRate(int fr)=0;

					/**
					 * set the minimum frame rate.
					 * @param min [in]the minimum
					 * @note supported BitRateControl:\n
					 * 	 	DBRC:\n
					 * 	 	ABRC:\n
					 * @see BitRateControl
					 */
					virtual void SetFrameRateMinimum(int min)=0;

					/**
					 * set the maximum frame rate.
					 * @param max [in]the maximum
					 * @note supported BitRateControl:\n
					 * 	 	DBRC:\n
					 * 	 	ABRC:\n
					 * @see BitRateControl
					 */
					virtual void SetFrameRateMaximum(int max)=0;

				public:


					/**
					 * set the minimum bit rate(BR).
					 * @param min [in]the minimum
					 * @note supported BitRateControl:\n
					 * 	 	ABRC:\n
					 * @see BitRateControl
					 */
					virtual void SetBitRateMinimum(int min)=0;

					/**
					 * set the maximum bit rate(BR).
					 * @param max [in]the maximum
					 * @note supported BitRateControl:\n
					 * 	 	ABRC:\n
					 * @see BitRateControl
					 */
					virtual void SetBitRateMaximum(int max)=0;

				public:
					/**
					 * manual set the quantization parameter.
					 * @param qp [in] quantization parameter. \n
					 * 		min: 10 \n
					 * 		max: 50 \n
					 * @note supported BitRateControl:\n
					 * 	 	MQPC: \n
					 * @see BitRateControl
					 */
					virtual void SetQP(int qp)=0;

					/**
					 * set the minimum quantization parameter(QP).
					 * @param min [in]the minimum
					 * @note supported BitRateControl:\n
					 * 	 	MBRC:\n
					 * 	 	DBRC:\n
					 * 	 	ABRC:\n
					 * @see BitRateControl
					 */
					virtual void SetQPMinimum(int min)=0;

					/**
					 * set the maximum quantization parameter(QP).
					 * @param max [in]the maximum
					 * @note supported BitRateControl:\n
					 * 	 	MBRC:\n
					 * 	 	DBRC:\n
					 * 	 	ABRC:\n
					 * @see BitRateControl
					 */
					virtual void SetQPMaximum(int max)=0;

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