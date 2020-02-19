//
// Created by jg.wang on 2020/1/21.
//

#pragma once

#include "_object_base.h"
#include <vector>
#include <memory>
#include <string>

namespace omf {
	namespace api {
		class OmfObject :public object_base{
		public:
			OmfObject();
			OmfObject(const char*);
			OmfObject(const std::string&);
		public:
			operator bool() const;
			bool Valid() const;
		public:
			bool Set(const char*key,void*val,const std::type_info&);
			bool Get(const char*key,void*val,const std::type_info&);
			bool HasProperty(const char*key);

		public:
			bool From(const char*);
			bool To(std::string&);
			std::string To();
		public:
			class AttrInfo{
			public:
				const char* name =0;
				const std::type_info* type=0;
				int size=0;
				const char* script=0;
				bool canget=false;
				bool canset=false;
			};
			bool GetPropertyInfo(const char*key,AttrInfo&info);
			std::vector<const char*> GetProperties() const;
		public:
			template<class T>
			bool Set(const char*key,T val){return Set(key,&val,typeid(T));}
			template<class T>
			bool Get(const char*key,T val){return Get(key,&val,typeid(std::decay<T>::type));}
		public:
			bool SendMessage(const char*);
			using MessageCallback=std::function<bool(const char*)>;
			bool Register(const MessageCallback&);

		public:
			const char* Status();
			bool Status(const char*);
			bool StatusUp(const char*);
			bool StatusDown(const char*);

		protected:
			void* _obj=0;
			std::shared_ptr<object_base> _obj0;
		public:
			std::shared_ptr<object_base> get_obj()const;
		};
	}
}
