//
// Created by wang.zhou on 2020/3/13.
//

#pragma once
namespace omf{
	namespace rtmp{
		class OmfRtmpClient {
		protected:
			OmfRtmpClient()= default;
		public:
			virtual ~OmfRtmpClient(){}
		public:
			virtual bool Start() = 0;
			virtual bool Stop() = 0;
		public:
			virtual bool SetSourceObject(void*) = 0;
		public:
			static OmfRtmpClient* CreateNew(const char* cfg=0,const char*url=0);
		};
	}
}



