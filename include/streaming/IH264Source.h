//
// Created by jg.wang on 2019/6/28.
//

#pragma once
#include "common/IStreamControl.h"
#include "common/IBitRateControl.h"
#include "common/MediaInfo.h"
#include "common/IVideoSource.h"
#include "common/ITrigger.h"
#include "common/IH264.h"
namespace omf {
	namespace api {
		namespace streaming {
			/**
			 *@example _demoH264Source.cpp
			 */
			/**
			 * @brief provide h264 date source.
			 * You can get H264 realtime streaming from the interface.\n
			 * Firstly,create an instance.\n
			 * 	CreateNew(const char *keywords).\n
			 * Secondly,set the parameters.\n
			 * 	YUV source: \n
			 *  	SelectSensor(int id) \n
			 *  	SetWidth(int w) \n
			 *  	SetHeight(int h) \n
			 * 	H264 encoder: \n
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
			 *  GetH264MediaInfo(). call after Open().\n
			 *  GetCurrentFrameRate(). call after Start().\n
			 *  GetCurrentBitRate(). call after Start().\n
			 *  GetCurrentQP(). call after Start().\n
			 * Others,\n
			 *  ForceKeyFrame(). call after start().\n
			 */
			class IH264Source
				: virtual public common::IStreamSource
				, virtual public common::IBitRateControl
				, virtual public common::IVideoSource
				, virtual public common::ITrigger
				, virtual public common::IH264
			{
			public:
				using H264MediaInfo=omf::api::streaming::common::H264MediaInfo;
			public:
				/**
				 * create a new H264Souce instance.
				 * @param keywords[in] the keywords for selecting a H264Souce type.
				 * @return the new H264Souce instance.
				 */
				static IH264Source* CreateNew(const char *keywords);
			};
		}
	}
}




