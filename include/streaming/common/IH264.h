//
// Created by jg.wang on 2019/8/6.
//
#pragma once

#include "common/MediaInfo.h"
#include "common/IVideo.h"
namespace omf {
	namespace api {
		namespace streaming {
			namespace common{
				class IH264:public virtual IVideo{
				public:
					/**
					 * set the GOP(Group Of Pictures).
					 * @param v [in] the GOP numbers.
					 */
					virtual void SetGop(int v)=0;
					/**
					 * set the coded frame sequence structure.
					 * @param v [in] the coded frame sequence structure string.\n
					 * 		"IIII": I I I I I I....\n
					 * 		"IPPP": I P P ... I P P ...\n
					 * 		"IBBP": I B B P B B P ... I B B P ...\n
					 */
					virtual void SetGopType(const char *type)=0;
				};
			}
		}
	}
}