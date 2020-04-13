#include "RtspSession.h"
#include "omf_api.h"
#include "OmfDbg.h"
#include "_hash.h"
#include "OmfAttrSet.h"
#include <algorithm>
#include "RtspSmssH264.h"
#include "RtspSmssAac.h"
//#undef dbgEntryTest
//#define dbgEntryTest(s) //dbgEntrySky(s)
using namespace omf;
using namespace omf::rtsp;
using namespace omf::api;
RtspSession::RtspSession(UsageEnvironment&env,const char* cfgs,const char*name)
	:ServerMediaSession(env,name,0,"fw sms",false,0)
{	dbgTestPVL(name);
	dbgTestPVL(cfgs);
	///
	//OmfAttrSet ap0(cfgs);
	//auto keys=ap0.Keys();
	//for(auto codec:keys){dbgTestPVL(codec);
	//	auto vals=ap0.Get(codec);dbgTestPVL(vals);
	//	if(vals) {
	//		std::shared_ptr<OmfAttrSet> ap(new OmfAttrSet(vals));
	//		switch(::Hash(codec)){
	//			case ::Hash("h264"):
	//				addSubsession(new RtspSmssH264(env,ap));
	//				break;
	//			case ::Hash("aac"):
	//				addSubsession(new RtspSmssAac(env,ap));
	//				break;
	//			default:
	//				dbgErrPSL("unsupport codec:"<<codec);
	//				break;
	//		}
	//	}
	//}
}//

RtspSession::~RtspSession(){
	//dbgTestPVL(name);
}

RtspSession* RtspSession::CreateNew(UsageEnvironment& env,const char* cfgs,const char*url){
	std::shared_ptr<OmfAttrSet> ap0(new OmfAttrSet(cfgs));
	returnIfErrC(0,!ap0);
	auto session=new RtspSession(env,cfgs,url);
	auto ap=ap0;
	while(ap){
		auto codec = ap->Get("codec");
		if(codec){
			switch(::Hash(codec)){
				case ::Hash("h264"):
					session->addSubsession(new RtspSmssH264(env,ap));
					break;
				case ::Hash("aac"):
					session->addSubsession(new RtspSmssAac(env,ap));
					break;
				default:
					dbgErrPSL("unsupport codec:"<<codec);
					break;
			}
		}
		ap=ap->Next();
	}

	if(!session->numSubsessions()){
		dbgErrPSL("invalid streaming in cfgs:"<<cfgs);
		delete session;
		session=0;
	}
	return session;
//	return new RtspSession(env,cfgs,url);
#if 0
	OmfAttrSet ap0(cfgs);
	auto keys=ap0.Keys();
	for(auto key:keys){
		auto vals=ap0.Get(key);
		if(vals) {
			OmfAttrSet ap(vals);
			auto codec = ap.Get("codec");
			if(codec){
				switch(::Hash(codec)){
					case ::Hash("h264"):
						break;
					case ::Hash("aac"):
						break;
					default:
						dbgErrPSL("unsupport codec:"<<codec);
						break;
				}
			}
		}
	}
#endif
#if 0
	dbgInfoPSL(app0->Name());
	Application* app = dynamic_cast<Application*>(app0);
	returnIfErrC(0,!app);
	std::vector<PacketRing*> rsts;
	app->OutputList(rsts,true);
	dbgTestPVL(rsts.size());
	returnIfErrC(0,!app->StatusChange(IStatus::ready));
	RtspSession* sms = new RtspSession(env,app,name);
	for(auto ring:rsts){
		const char* codec = ring->Media()->Codec();
		std::string type("rtsp-smss-");type+=codec;
		auto smss = Object::New<ServerMediaSubsession>(type.c_str(),0,0,(object*)0,&env,ring);
		if(smss){
			sms->addSubsession(smss);
		}else{
			dbgErrPSL(type);
		}
	};
	returnIfErrC(0,!app->StatusChange(IStatus::play));
	//sms->deleteWhenUnreferenced()=True;
#endif
	//return sms;
}
