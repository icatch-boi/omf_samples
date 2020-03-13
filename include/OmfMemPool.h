//
// Created by jg.wang on 2020/3/2.
//
#pragma once

namespace omf{
	namespace mem{
		class OmfMemPool{
		public:
			virtual ~OmfMemPool();
		public:
			virtual int MaxSize() const;
		public:
			virtual void* Alloc(int size)=0;
			virtual bool Free(void* ptr,int size=0)=0;
		public:	///named memory
			virtual bool IsNamed()const;
			virtual void* Alloc(int size,unsigned key)=0;
			virtual void* Alloc(int size,const char*name)=0;
			virtual bool Free(const char*name)=0;
			virtual bool Free(unsigned key)=0;
		public:
			static OmfMemPool* const Globle(const char*keywords);
			static OmfMemPool* const ShmPool();
			static OmfMemPool* const BigPool();
		};
	}
}
