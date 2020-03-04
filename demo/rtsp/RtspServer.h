
#pragma once
#include "RTSPServerSupportingHTTPStreaming.hh"
#include <initializer_list>
#include "OmfAttrSet.h"
namespace omf{
namespace rtsp {
	class RtspServer :
			public RTSPServerSupportingHTTPStreaming
	{

	public:
		RtspServer(UsageEnvironment&,int ourSocket,Port,UserAuthenticationDatabase*,const char*url_map=0);
		virtual ~RtspServer();
	public:
		virtual ServerMediaSession*lookupServerMediaSession(char const*,Boolean);
	public:
		static RtspServer*createNew(UsageEnvironment&
			,std::initializer_list<Port>
			,UserAuthenticationDatabase*user = 0
			,const char*url_map=0
		);
		static int Socket(UsageEnvironment&,Port);

	protected:
		api::OmfAttrSet _url_map;

	};
}
}