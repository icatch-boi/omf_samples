//
// Created by jg.wang on 2019/8/6.
//
#pragma once

#include "common/MediaInfo.h"

namespace omf {
	namespace api {
		namespace streaming {
			namespace common{
				class IH264{
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

				public:
					/**
					 * force h264 encoder to output the key frame.
					 */
					virtual void ForceKeyFrame()=0;
				public:
					/**
					 * get the output H264 media info.
					 * @return H264 media info.
					 * @see H264MediaInfo
					 */
					virtual common::H264MediaInfo GetH264MediaInfo() const =0;
				};
			}
		}
	}
}