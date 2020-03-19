//
// Created by jg.wang on 2019/7/4.
//
#include "IVideo.h"
#include "MediaInfo.h"
#pragma once
namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				class IYuvDevice:public virtual IVideo {
				public:
					using YuvMediaInfo=omf::api::streaming::common::YuvMediaInfo;
				public:
					/**
					 * set the width of image.
					 * @param w [in]the width \n
					 */
					//virtual void SetWidth(int w)=0;

					/**
					 * set the height of image.
					 * @param h [in]the height \n
					 */
					//virtual void SetHeight(int h)=0;
					/**
					 * set the yuv format.
					 * @param fmt [in]the yuv format string. \n
					 * 		default: "420v50", this format provides the lowest memory consumption. \n
					 * 		"422nv16": the 422 pixel format and the nv16 layout, used for external editing. \n
					 * 		"420v50" : the 420 pixel format and icatos compressed layout, used for internal encoder. \n
					 */
					virtual void SetFormat(const char *fmt)=0;
					/**
					 * set the interlaced for YUV.
					 * @param interlaced [in] \n
					 * 		default: false \n
					 * 		true: interlaced \n
					 * 		false: uninterlaced \n
					 */
					virtual void SetInterlaced(bool interlaced)=0;
					/**
					 * set the framerate.
					 * @param frame per seconds.
					 */
					virtual void SetFrameRate(int fr)=0;
				public:
					/**
					 * get the output YUV media info.
					 * @return YUV media info.
					 * @see YuvMediaInfo
					 */
					virtual common::YuvMediaInfo GetYuvMediaInfo() const =0;

				};
			}
		}
	}
}