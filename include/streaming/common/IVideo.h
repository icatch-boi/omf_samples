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

				public:
					/**
					 * Flip horizontally/vertically
					 * @param hflip : Flip horizontally
					 * @param vflip : Flip vertically
					 * @return true/false
					 */
					virtual bool SetFlip(bool hflip,bool vflip)=0;
				};
			}
		}
	}
}
