//
// Created by wang.zhou on 2020/3/13.
//

#pragma once

#include "OmfObject.h"

namespace omf{
	namespace rtmp{
		class OmfRtmpClient {
		protected:
			OmfRtmpClient(const char* cfg);
		public:
			virtual ~OmfRtmpClient();
		public:
			virtual bool Start();
			virtual bool Stop();
		public:
			static OmfRtmpClient* CreateNew(const char* cfg=0,const char*url=0);
		};
	}
}



