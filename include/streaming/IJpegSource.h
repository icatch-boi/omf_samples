//
// Created by jg.wang on 2019/6/28.
//

#pragma once
#include "common/IEncoder.h"
#include "common/IVideoSource.h"
namespace omf {
	namespace api {
		namespace streaming {
			/**
			 *@example _demoJpegSource.cpp
			 */
			/**
			 * @brief provide Jpeg date source.
			 * You can get Jpeg realtime streaming from the interface.\n
			 * Firstly,create an instance.\n
			 * 	CreateNew(const char *keywords).\n
			 * Secondly,set the parameters.\n
			 * 	YUV source: \n
			 *  	SelectSensor(int id) \n
			 *  	SetWidth(int w) \n
			 *  	SetHeight(int h) \n
			 * 	Jpeg encoder: \n
			 * 		SetGop(int gop)\n
			 * 		SetGopType(const char* type)\n
			 * 	BitRate Control:\n
			 * 		IsSupportConstantBitRateControl()\n
			 *		IsSupportManualBitRateControl()\n
			 *		IsSupportAdaptBitRateControl()\n
			 *		IsSupportAdaptBitRateControl()\n
			 *		IsSupportManualFrameRateControl()\n
			 * Thirdly,	register streaming callbck.\n
			 * 	RegisterMessageCallback(const FuncMessage &func)\n
			 * 	RegisterOutputCallback(const IStreamEntry::FuncFrame &func)\n
			 * Finally, operator streaming status.\n
			 * 	Open()\n
			 * 	Close()\n
			 * 	Start()\n
			 * 	Stop()\n
			 * Moreover, get same information from the instance.\n
			 *  GetJpegMediaInfo(). call after Open().\n
			 *  GetCurrentFrameRate(). call after Start().\n
			 *  GetCurrentBitRate(). call after Start().\n
			 *  GetCurrentQP(). call after Start().\n
			 * Others,\n
			 *  ForceKeyFrame(). call after start().\n
			 */
			class IJpegSource
				: virtual public common::IVideoSource
				, virtual public common::IEncoder
			{
			public:
				using JpegMediaInfo=omf::api::streaming::common::JpegMediaInfo;
			public:
				/**
				 * get the output Jpeg media info.
				 * @return Jpeg media info.
				 * @see JpegMediaInfo
				 */
				virtual common::JpegMediaInfo GetJpegMediaInfo() const =0;
			public:
				/**
				 * create a new JpegSouce instance.
				 * @param keywords[in] the keywords for selecting a JpegSouce type.
				 * @return the new JpegSouce instance.
				 */
				static IJpegSource* CreateNew(const char *keywords);
				static IJpegSource* CreateNewFromConfig(const char *config);
				static IJpegSource* CreateNewFromFile(const char *file);
			};
		}
	}
}




