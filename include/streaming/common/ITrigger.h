//
// Created by jg.wang on 2019/7/31.
//

#pragma once
namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				class ITrigger {
				public:
					virtual bool IsSupportSingleFrameTrigger() const=0;
					virtual bool Trigger()=0;
				};
			}
		}
	}
}
