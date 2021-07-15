//
// Created by jg.wang on 2020/2/11.
//

#pragma once

#include "_string_base.h"
#include <stdlib.h>
#include <getopt.h>
#include <type_traits>
#include <functional>
#include <vector>
#include <iostream>
namespace omf {
	class OmfHelper {
	public:
		class Item;
	public:
		/**
		 * Add parser list.
		 * @param opts[in] the parser list.
		 */
		OmfHelper(Item*opt=0);
		/**
		 * Add parser list and parse the input paramers.
		 * @param opts[in]: the parser list.
		 * @param argc [in]: the input paramers number.
		 * @param argv [in]: the input paramers.
		 */
		OmfHelper(Item*opt,int argc,char **argv);
		~OmfHelper();
	protected:
		bool _output=true;
		bool _valid=true;
		///
		bool _dbg=false;
		const char* _log=0;
		const char* _display=0;
		///
		void init();
		void AddDefault();
	public:
		/**
		 * Check the Helper is valid or not.
		 * @return true/false
		 */
		operator bool()const;
		/**
		 * Add Parser list.
		 * @return true/false
		 */
		bool Add(Item*);
		/**
		 * Parse the paramers.
		 * @param argc [in]: the input paramers number.
		 * @param argv [in]: the input paramers.
		 * @return true/false
		 */
		bool Parse(int argc,char **argv);
		/**
		 * output the parser list.
		 */
		void Helper();
		/**
		 * output the paramers value after parsed.
		 */
		void Print()const;
		/**
		 * return the debug switch:--dbg or -D
		 * @return true/false
		 */
		bool Debug()const;
		/**
		 * return the log configer:--log or -L
		 * @return log configure string
		 */
		const char* Log()const;
		/**
		 * return the display configer:--show or -S
		 * @return display configure string
		 */
		const char* Display()const;
	protected:
		char _short_options[1024];
		struct option _long_options[128];
		std::vector<Item> _options;
	protected:
		using FuncPrint=std::function<void()>;
		using FuncProcess=std::function<void(char*)>;
		using FuncVoid=std::function<void()>;
		FuncProcess FindProcess(char key);

	public:
		class Item{
		public:
			/**
			 * the default constructer.
			 * usually, it means the end of the list.
			 */
			Item();
			/**
			 * the groups constructer.
			 * usually, it means the begin of params group.
			 * @param description
			 */
			Item(const char* description);
			/**
			 * the full constructer.
			 * Use this constructor to indicate that this item takes a post parameter.
			 * @param longname [in]:the full paramer name: --xxx
			 * @param shortname [in]:the short keys: -x
			 * @param process [in]:the paramer process callback.
			 * @param format [in]:the paramer type.
			 * @param value [in]:the paramer default value.
			 * @param description [in]:the paramer description.
			 */
			Item(const char*longname,char shortname
					,FuncProcess process
					,const char*format
					,const char*value
					,const char* description = 0
			);
			/**
			 * Use this constructor to indicate that this item does not have post parameters.
			 * @param longname [in]:the full paramer name: --xxx
			 * @param shortname [in]:the short keys: -x
			 * @param process [in]:the paramer process callback.
			 * @param description [in]:the paramer description.
			 */
			Item(const char*longname,char shortname
					,FuncVoid process
					,const char* description = 0
			);
			/**
			 * the template constructer.
			 * @tparam T :the paramer type
			 * @param longname [in]:the full paramer name: --xxx
			 * @param shortname [in]:the short keys: -x
			 * @param t [in]:the reference to the receive variable of the input parameter
			 * @param descript [in]:the paramer description.
			 */
			template<class T>
			Item(const char*longname,char shortname,T&t,const char* descript=0){//dbgNotePVL(longname);dbgNotePVL(TypeName(typeid(T)));
				init(longname,shortname
						,[&t](char*s) {	from_string(s,t);}
						,TypeName(typeid(T))
						,to_string(t)
						,descript
					 	,[&t,longname](){std::cout<<longname<<'='<<to_string(t)<<std::endl;}
				);
			}
		public:
			const char *long_name=0;
			char short_name=0;
			const char* para_format=0;
			std::string value_default;
			FuncProcess process;
			const char *descript=0;
			FuncPrint Print;
		protected:
			void init(const char*longname,char shortname
					,FuncProcess process
					,const char*format
					,const std::string& value
					,const char* descript
					,FuncPrint print=nullptr
			);
		};

	};
}


