//
// Created by jg.wang on 2019/11/29.
//
#pragma once
namespace omf {
	namespace rtsp {
		class IRtspService {
		public:
			virtual ~IRtspService(){}
		public:
			virtual bool Start()=0;
			virtual bool Stop()=0;
		public:
			static IRtspService* CreateNew(const char*urlmap=0,const char*auth=0);
		};
	}
}
