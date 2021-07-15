#pragma once
#include "ServerMediaSession.hh"
#include "rtsp.h"
namespace omf{
namespace rtsp {
	class RtspSession
		: public ServerMediaSession
	{
	protected:
		RtspSession(UsageEnvironment& env,const char* cfgs,const char*url);
	public:
		virtual ~RtspSession();
	//protected:
		//void* _app;
	public:
		static RtspSession* CreateNew(UsageEnvironment& env,const char* cfgs,const char*url);

		///to remove long-unused "ServerMediaSession"
		virtual void noteLiveness(){deleteWhenUnreferenced()=true;}
	};
}
}
