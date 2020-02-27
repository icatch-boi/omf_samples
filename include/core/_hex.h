//
// Created by jg.wang on 2020/2/25.
//

#pragma once

#include "_typedef.h"
#include <stdio.h>
#include <iostream>
namespace omf{
	class Hex{
	public:
		char _str[20];
	public:
		Hex(int8   v,bool has0x=true){init(v,has0x);}
		Hex(int16  v,bool has0x=true){init(v,has0x);}
		Hex(int32  v,bool has0x=true){init(v,has0x);}
		Hex(int64  v,bool has0x=true){init(v,has0x);}
		Hex(uint8  v,bool has0x=true){init(v,has0x);}
		Hex(uint16 v,bool has0x=true){init(v,has0x);}
		Hex(uint32 v,bool has0x=true){init(v,has0x);}
		Hex(uint64 v,bool has0x=true){init(v,has0x);}
		Hex(void*  v,bool has0x=true){init(v,has0x);}

		Hex(long   v,bool has0x=true){init(v,has0x);}
		Hex(unsigned long v,bool has0x=true){init(v,has0x);}
	public:
		////void operator>>(ostream&os){os << setw(_width) << setfill(_fill) << hex << _val;}
		template<class T>
		void init(T t,bool has0x){
			sprintf(_str,patternd[has0x?1:0][sizeof(T)>>1],t);
		}

	protected:
		static const char* patternd[2][5];
	};
	static inline std::ostream& operator<<(std::ostream&os,const omf::Hex&v){
		return os<<v._str;
	}
}