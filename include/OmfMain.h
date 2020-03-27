//
// Created by jg.wang on 2019/11/28.
//
#pragma once

#include "_object_base.h"
#include <string>
#include <functional>
namespace omf {
	namespace api {
		class OmfMain {
		public:
			/** the OMF Process Object, the OMF Process must have exactly one this instance.
			 * @param para :the omfInit paramter, usually is nullptr.
			 */
			OmfMain(const char *para = 0);
			/**
			 *
			 */
			~OmfMain();
		public:
			/** the omf globel command interface.
			 * @param cmd : the cmd string
			 * @param input : the input serialization paramter.
			 *  /li default is nullptr
			 * @param output : the output serialization parametr by cmd function.
			 *  /li default is nullptr
			 *  /li "rst=true/false" : the return value of cmd function.
			 * @return bool:
			 * 	/li true: cmd is existed and executed.
			 * 	/li false: cmd is not existed.
			 */
			bool Command(const char*cmd,const char*input=0,char* output=0);
			/** the omf globel command interface.
			 * @param cmd : the cmd string
			 * @param input : the input serialization paramter.
			 * @param output : the output serialization parametr by cmd function.
			 *  /li "rst=true/false" : the return value of cmd function.
			 * @return bool:
			 * 	/li true: cmd is existed and executed.
			 * 	/li false: cmd is not existed.
			 */
			bool Command(const char*cmd,const char*input,std::string& output);

			///
			using CommandCallback = std::function<bool(const char*input,std::string&output)>;
			/** register cb to omf system.
			 * @param cmd: the cmd keywords.
			 * @param cb : the reigstered callback.
			 * @return
			 */
			bool Register(const char* cmd,const CommandCallback&cb);
			///
			using CommandCallback1 = std::function<bool(const char*input)>;
			/** register cb to omf system.
			 * @param cmd: the cmd keywords.
			 * @param cb : the reigstered callback.
			 * @return
			 */
			bool Register(const char* cmd,const CommandCallback1&cb);
		public:
			/**
			 * output the load omf modules.
			 * @return true/false
			 */
			bool ShowModules();
			/**
			 * configure the omf log,
			 * @return true/false
			 */
			bool LogConfig(const char*);
			/**
			 * enable/disable the debug model.
			 * @param en : true/false
			 * @return true/false
			 */
			bool Debug(bool en);
			/** register SIGINT process function.
			 * @param en :true/false
			 * @return true/false
			 */
			bool SignalINT(bool en);
		public:
			bool Register(const char* name,std::function<object_base*()>,std::type_info*type);
			template<class T>
			bool Register(const char* name){
				return Register(name,[](){return new T();},&typeid(T));
			}
		};
	}
}

