//
// Created by jg.wang on 2019/7/31.
//

#pragma once

#include "IVideo.h"
#include "IStreamControl.h"
#include "IEncoder.h"
namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				class IVideoSource
					:public virtual IStreamSource
					,public virtual IVideo
				{
				public:
					using VideoMediaInfo=omf::api::streaming::common::VideoMediaInfo;
				public:
					/**
					 * Select the sensor with ID.
					 * @param id [in] sensor ID.\n
					 * 		default: 0\n
					 * 		v37 support: 0,1,2 \n
					 */
					virtual void SelectSensor(int id)=0;
				};
			}
		}
	}
}
