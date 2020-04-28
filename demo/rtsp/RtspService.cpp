#include "RtspService.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "BasicUsageEnvironment.hh"

#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntryPink(s)
///
extern void vfp_enable();
extern void vfp_disable();
using namespace omf::rtsp;
using namespace omf::api;

//DEFINE_REGISTER(RtspService) {
//	TClass<>::Register<RtspService>(name ? name : "service-rtsp-server-live555", {
//		new TProperty("auth",&RtspService::AddAuth),
//		new TProperty("url_map",&RtspService::_url_map),
//	})->Init(LIBRARY, VERSION(0), "iCatchtek", "jg.wang");
//}

RtspService::RtspService(const char*urlmap,const char*auth)
    : _server(0)
	, _auth(0)
{
	if(urlmap)
		_urlmap=urlmap;
	if(auth)
		AddAuth(auth);
	returnIfErrC0(!Open());
}

RtspService::~RtspService() {
	if(_sts==State::play)Stop();
	Close();
}
bool RtspService::AddAuth(const char*str){
	OmfAttrSet ap(str);
	auto keys = ap.Keys();
	for(auto key:keys){
		auto auth = key;
		auto keyword = ap.Get(key);
		if(!_auth)_auth= new UserAuthenticationDatabase;
		_auth->addUserRecord(auth,keyword);
	}
	return true;
}
bool RtspService::Open() {
	std::unique_lock<std::mutex> lck(_mtx);
	returnIfErrC(true,_sts!=State::null);
	_sts=State::ready;
	// Begin by setting up our usage environment:
	_scheduler = BasicTaskScheduler::createNew();
	_env = BasicUsageEnvironment::createNew(*_scheduler);

	// Create the RTSP server.
	_server = RtspServer::createNew(*_env,{554,8554},_auth,_urlmap.c_str());
	returnIfErrC(false,!_server);
	char *url = _server->rtspURLPrefix();
	dbgNoteSL("\nRtspServer()/Live555("
			<<LIVEMEDIA_LIBRARY_VERSION_STRING
			<<"):\n"
			<<_server->rtspURLPrefix());

	// create a HTTP server for RTSP-over-HTTP tunneling.
	if (_server->setUpTunnelingOverHTTP(80) ||
		_server->setUpTunnelingOverHTTP(8000) ||
		_server->setUpTunnelingOverHTTP(8080)) {
		dbgNoteSL("RTSP-over-HTTP tunneling:"<<_server->httpServerPortNum());
	} else {
		dbgNoteSL("RTSP-over-HTTP tunneling is not available.");
	}

	return true;
}

bool RtspService::Close() {
	std::unique_lock<std::mutex> lck(_mtx);
	returnIfErrC(true,_sts!=State::ready);
	_sts=State::null;
	///
	if(_scheduler) {
		delete _scheduler;
		_scheduler=0;
	}
	if(_auth) {
		delete _auth;
		_auth = 0;
	}
	if (_env) {
		_env->reclaim();
		_env=0;
	}
    return true;
}
bool RtspService::Start() {
	std::unique_lock<std::mutex> lck(_mtx);
	returnIfErrC(true,_sts!=State::ready);
	_sts=State::play;
	///
	_thread.reset(new std::thread([this]() {
		vfp_enable();
		_env->taskScheduler().doEventLoop();
		vfp_disable();
	}));
	return true;
}
bool RtspService::Stop() {
	std::unique_lock<std::mutex> lck(_mtx);
	returnIfErrC(true,_sts!=State::play);
	_sts=State::ready;
	///
	if (_server)
		Medium::close(_server);
	_thread->join();
	return true;
}

