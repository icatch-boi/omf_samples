//
// Created by jg.wang on 2020/2/12.
//
#pragma once

#include <functional>
namespace omf{
	class ExitCall{
	public:
		ExitCall(){}
		template<class F>
		ExitCall(F f):_func(f){}
		~ExitCall(){if(_func)_func();}
		void Reset(){_func= nullptr;}
		template<class F>
		void Reset(const F& f){_func= f;}
	protected:
		std::function<void()> _func;
	};
}
