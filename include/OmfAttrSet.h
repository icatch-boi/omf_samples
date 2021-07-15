//
// Created by jg.wang on 2019/11/28.
//

#pragma once
#include <string>
#include <vector>
#include <memory>
#include "_object_base.h"

#define OMF_ATTR_METHOD(type) \
bool Set(const char*key,type val);\
bool Add(const char*key, type val);\
bool SetOrAdd(const char*key,type val);\
bool Get(const char*key,type& val)const;\
type GetOrDef(const char*key,type def)const;

namespace omf{
	namespace api{
		class OmfAttrSet:public object_base{
		public:
			/**
			 * create from string.
			 * @param str [in]:the serialize string
			 */
			OmfAttrSet(const std::string& str);
			/**
			 * create from string.
			 * @param str [in]:the serialize string
			 */
			OmfAttrSet(const char* str=0);
		private:
			OmfAttrSet(void*);
		public:
			/**
			 * Check is Valid or not.
			 * @return true/false
			 */
			operator bool() const;
		public:
			std::shared_ptr<OmfAttrSet> Next();
		protected:
			void* _obj=0;
			std::shared_ptr<object_base> _obj0;
		public:
			std::shared_ptr<object_base> get_obj() const;
		public:
			/**
			 * Parse a serialize string.
			 * @param str [in]:the serialize string
			 * @return true/false
			 */
			bool Parse(const std::string& str);
			/**
			 * Parse a serialize string.
			 * @param str [in]:the serialize string
			 * @return true/false
			 */
			bool Parse(const char* str);
		public:
			/**
			 * Check contain key or not.
			 * @param key [in]:the key of attribute.
			 * @return true/false
			 */
			bool Contain(const char*key)const;
			/**
			 * List keys for all attributes
			 * @return
			 */
			std::vector<const char*> Keys()const;
		public:
			/**
			 * Get the attribute with the key.
			 * @param key
			 * @return the value of attribute.
			 */
			const char*Get(const char*key)const;
		public:
			OMF_ATTR_METHOD(const char*);
			OMF_ATTR_METHOD(char*);
			OMF_ATTR_METHOD(unsigned char*);
			OMF_ATTR_METHOD(std::string);
			OMF_ATTR_METHOD(long long);
			OMF_ATTR_METHOD(unsigned long long);
			OMF_ATTR_METHOD(int);
			OMF_ATTR_METHOD(unsigned int);
			OMF_ATTR_METHOD(short);
			OMF_ATTR_METHOD(unsigned short);
			OMF_ATTR_METHOD(char);
			OMF_ATTR_METHOD(unsigned char);
			OMF_ATTR_METHOD(bool);
			OMF_ATTR_METHOD(void*);
		public:
			/**
			 * Set the value of the property that matches the entered key.
			 * @tparam T :the type of property value.
			 * @param key [in]:the key of property
			 * @param val [in]:the value of property
			 * @return true/false
			 */
			template<class T>
			bool Set(const char*key,T* val){
				return Set(key,(void*)val);
			}
			/**
			 * Add a property with the key-value pair.
			 * @tparam T :the type of property value.
			 * @param key [in]:the key of property
			 * @param val [in]:the value of property
			 * @return true/false
			 */
			template<class T>
			bool Add(const char*key, T* val){
				return Add(key,(void*)val);
			}
			/**
			 * if the property with key exists, set the new value to it.
			 * otherwise add a new property with the key-value pair.
			 * @tparam T :the type of property value.
			 * @param key [in]:the key of property
			 * @param val [in]:the value of property
			 * @return true/false
			 */
			template<class T>
			bool SetOrAdd(const char*key,T* val){
				return SetOrAdd(key,(void*)val);
			}
			/**
			 * Get the value of property.
			 * @tparam T :the type of property value.
			 * @param key [in]:the key of property
			 * @param val [out]:the value of property
			 * @return true/false
			 */
			template<class T>
			bool Get(const char*key,T*& val)const{
				return Get(key,(void*&)val);
			}
			/**
			 * Get the value of propery.
			 * Returns the value of the property if it exists, otherwise returns the given value.
			 * @tparam T :the type of property value.
			 * @param key [in]:the key of property
			 * @param def [in]:the given value of property
			 * @return the value of propery.
			 */
			template<class T>
			T* GetOrDef(const char*key,T* def)const{
				return GetOrDef(key,(void*)def);
			}
			/**
			 * deserialize the OmfAttrSet to string.
			 * @param is_ext_next [in]:
			 * @return the serialize string.
			 */
			std::string ToString(bool is_ext_next=false)const;
		};
	}
}
