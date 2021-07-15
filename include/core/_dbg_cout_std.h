#pragma once
#include <iostream>
#include "_dbg_api_def.h"
namespace omf{
	namespace dbg{
		namespace xxx {
			std::ostream &dbgCoutGet(int);
		}
	}
}
#define dbgCoutSelect(i) omf::dbg::xxx::dbgCoutGet(i)

