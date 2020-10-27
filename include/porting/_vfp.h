//
// Created by jg.wang on 2020/9/4.
//

#pragma once

#include "porting.h"
namespace omf{
	class VFPCtrl{
	public:
		VFPCtrl(){vfp_enable();}
		~VFPCtrl(){vfp_disable();}
	};
}
