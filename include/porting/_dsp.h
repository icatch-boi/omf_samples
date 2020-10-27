//
// Created by jg.wang on 2020/9/4.
//

#pragma once

#include "porting.h"
namespace omf{
	class DSPCtrl{
	public:
		DSPCtrl(){dsp_enable();}
		~DSPCtrl(){dsp_disable();}
	};
}
