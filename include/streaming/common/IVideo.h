//
// Created by jg.wang on 2019/7/11.
//

#pragma once
namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				class IVideo {
				public:
					/**
					 * set the width of image.
					 * @param w [in]the width \n
					 */
					virtual void SetWidth(int w)=0;

					/**
					 * set the height of image.
					 * @param h [in]the height \n
					 */
					virtual void SetHeight(int h)=0;
				};
			}
		}
	}
}
