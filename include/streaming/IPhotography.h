//
// Created by jg.wang on 2019/7/11.
//

#pragma once

#include "common/IStreamControl.h"
#include "common/IVideo.h"

namespace omf {
	namespace api {
		namespace streaming {
			/**
			 *@example _demoPhotography.cpp
			 */
			/**
			 * @brief Photography one frame to IO with url.
			 * You can trigger once to get one file from the interface.\n
			 * Firstly,create an instance.\n
			 * 	CreateNew(const char *keywords).\n
			 * Secondly,set the parameters.\n
			 * 	video source: \n
			 *  	\li SelectSensor(int id) \n
			 *  	\li SetWidth(int w) \n
			 *  	\li SetHeight(int h) \n
			 *  url:\n
			 *  	\li SetURL(const char*url)
			 * Thirdly,	register streaming callbck.\n
			 * 	\li RegisterMessageCallback(const FuncMessage &func)\n
			 * fourly, open and start streaming.\n
			 * 	\li Open()\n			 *
			 * 	\li Start()\n
			 * then, snap to get one frame.\n
			 * 	\li Snap()\n
			 *  \li Snap(const char*url)\n
			 * finally,stop and cloce streaming.\n
			 * 	\li Close()\n
			 * 	\li Stop()\n
			 */
			class IPhotography
				: public virtual common::IStreamControl
				, public virtual common::IVideo
			{
			public:
				virtual ~IPhotography() {}
			public:
				/**
				 * Select the sensor with ID.
				 * @param id [in] sensor ID.\n
				 * 		default: 0\n
				 * 		v37 support: 0,1,2 \n
				 */
				virtual void SelectSensor(int id)=0;

				/**
				 * set the QP value for the encoder.
				 * @param qp [in]
				 */
				virtual void SetQP(int qp) = 0;

				/**
				 * get the current QP of the encoder.
				 * @return QP.
				 */
				virtual int GetQP()const = 0;

			public:
				/**
				 * set the url for Snap(). \n
				 * valid if IsSupportSnap() = true.
				 * @param url [in] [protocol]://[path]/[name] \n
				 * 		\li file:///home/xxx/video.mp4 \n
				 * 		\li tcp://192.168.10.10:9999 \n
				 * 		\li udp://192.168.10.10:9999 \n
				 * 		\li socket://23456789 \n
				 * 		\li io://object=12344567 \n
				 * 		\li io://type=xxx,[para0=val0],[para1=val1]... \n
				 */
				virtual void SetURL(const char*url)=0;
				/**
				 * snap a frame to url given by SetSnapURL(const char*url).\n
				 * valid if IsSupportSnap() = true.
				 * @return true/false
				 */
				virtual bool Snap(bool wait=false)=0;
				/**
				 * snap a frame to the given url.\n
				 * valid if IsSupportSnap() = true.
				 * @param url[in] the protocol of url has to match with SetSnapURL(const char*url).
				 * @return true/false
				 */
				virtual bool Snap(const char*url,bool wait=false)=0;
			public:
				/**
				 * create a new YuvSource instance. \n
				 * @param keywords[in] the keywords for selecting a YuvSource type. \n
				 * @return the new YuvSource instance. \n
				 */
				static IPhotography *CreateNew(const char *);

			};
		}
	}
}
