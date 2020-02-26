//
// Created by jg.wang on 2020/2/25.
//

#pragma once

#include <string>
#include <iostream>
namespace omf{
	class Dump{
	public:
		Dump(void*,int);
		Dump(unsigned long long);
	public:
		std::string _str;
	protected:
		void init(void*,int);
	};
	static inline std::ostream& operator<<(std::ostream&os,const omf::Dump&v){
		return os<<v._str;
	}
}
