//
// Created by jg.wang on 2017/7/19.
//
#pragma once

#include "stddef.h"
#include <typeinfo>
namespace omf {
	class object_base {
	public:
		virtual ~object_base();

		static void *operator new(size_t);

		static void operator delete(void *, size_t);

	public:
		template<class T = object_base>
		static bool Compare(const T *tag0, const T *tag1) { return *tag0 < *tag1; }

	public:
		inline const std::type_info &GetType() const { return typeid(*this); }

		template<class T>
		inline T *AsType() const { return dynamic_cast<T *>((object_base *) this); }

		template<class T>
		inline bool IsType() const { return AsType<T>(); }
	};

}
