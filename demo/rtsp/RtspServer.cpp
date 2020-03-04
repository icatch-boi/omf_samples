#include "RtspServer.h"
#include "RtspSession.h"
#include "BasicUsageEnvironment.hh"
#include "omf_api.h"
#include "OmfDbg.h"
//#undef dbgEntryTest
//#define dbgEntryTest(s) dbgEntrySky(s)
using namespace omf::rtsp;
///
RtspServer*RtspServer::createNew(UsageEnvironment &env
	,std::initializer_list<Port> ports
	,UserAuthenticationDatabase *user
	,const char*url_map
) {
	for(auto port:ports){
		int skt = setUpOurSocket(env, port);
		if (skt>0)
			return new RtspServer(env,skt,port,user,url_map);
	}
	dbgErrPSL("can't open port");
	return 0;
}
int RtspServer::Socket(UsageEnvironment&env,Port ourPort){
	return setUpOurSocket(env, ourPort);
}
///
RtspServer::RtspServer(UsageEnvironment& env
	,int ourSocket
	,Port ourPort
	,UserAuthenticationDatabase* authDatabase
	,const char*url_map
)
    : RTSPServerSupportingHTTPStreaming(env,ourSocket,ourPort,authDatabase,65)
{
	if(url_map) {
		_url_map.Parse(url_map);
		//dbgTestPVL(url_map);
		auto keys=_url_map.Keys();
		for(auto&key:keys){
			dbgNoteSL(key<<"==>"<<_url_map.Get(key));
		}
	}
}
///
RtspServer::~RtspServer() {
}
///
ServerMediaSession* RtspServer
::lookupServerMediaSession(char const *url,
						   Boolean isFirstLookupInSession) {
	dbgTestPVL(url);
	dbgTestPVL((bool)isFirstLookupInSession);
	ServerMediaSession* sms =
		RTSPServerSupportingHTTPStreaming::lookupServerMediaSession(url,isFirstLookupInSession);
	if(sms)return sms;
	//dbgTestPVL(url);
	const char* cfgs = _url_map.Get(url);//_url_map.GetOrDef(url,url);
	//if(cfgs)cfgs=url;
	dbgTestPVL(cfgs);
	returnIfErrCS(0,!cfgs,url<<','<<_url_map.ToString());

	sms = RtspSession::CreateNew(envir(),cfgs,url);
	if(sms)
		addServerMediaSession(sms);

	return sms;
}


