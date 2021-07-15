//
// Created by jg.wang on 2019/6/28.
//

#pragma once

#include <vector>
#include "common/MediaInfo.h"
#include "common/IStreamControl.h"
#include "common/IYuv.h"
#include "common/IVideoSource.h"
namespace omf {
	namespace api {
		namespace streaming {
			/**
			 *@example _demoYuvSource.cpp
			 */
			/**
			 * @brief provide YUV date source.
			 * You can get YUV realtime streaming from the interface.\n
			 * Firstly,create an instance.\n
			 * 	CreateNew(const char *keywords).\n
			 * Secondly,set the parameters.\n
			 * 	YUV source: \n
			 *  	SelectSensor(int id) \n
			 *  	SetWidth(int w) \n
			 *  	SetHeight(int h) \n
			 *  	SetFormat(const char *fmt) \n
			 *  	SetInterlaced(bool interlaced)\n
			 * Thirdly,	register streaming callbck.\n
			 * 	RegisterMessageCallback(const FuncMessage &func)\n
			 * 	RegisterOutputCallback(const IStreamEntry::FuncFrame &func)\n
			 * Finally, operator streaming status.\n
			 * 	Open()\n
			 * 	Close()\n
			 * 	Start()\n
			 * 	Stop()\n
			 * Moreover, get same information from the instance.\n
			 *  GetYuvMediaInfo(). call after Open().\n
			 */
			class IYuvSource
				: public virtual common::IVideoSource
				, public virtual common::IYuv
			{
			public:
				using YuvMediaInfo=omf::api::streaming::common::YuvMediaInfo;
			public:
				/**
				 * get the output YUV media info.
				 * @return YUV media info.
				 * @see YuvMediaInfo
				 */
				virtual YuvMediaInfo GetYuvMediaInfo() const =0;
			public:
				/**
				 * create a new YuvSource instance. \n
				 * @param keywords[in] the keywords for selecting a YuvSource type. \n
				 * @return the new YuvSource instance. \n
				 */
				static IYuvSource *CreateNew(const char *keywords);
				static IYuvSource* CreateNewFromConfig(const char *config);
				static IYuvSource* CreateNewFromFile(const char *file);
			public:
				/**
				 * get the IPcmSource capabilities list.
				 * @param keywords [in] the keywords for selecting a IPcmSource device.
				 * @return the capabilities list.
				 */
				static std::vector<YuvMediaInfo> GetCapabilities(const char*keywords);

			};
		}
	}
}

