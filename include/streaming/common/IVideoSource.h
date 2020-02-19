//
// Created by jg.wang on 2019/7/31.
//

#pragma once

#include "IVideo.h"
namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				class IVideoSource
					:public virtual IVideo
				{
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
