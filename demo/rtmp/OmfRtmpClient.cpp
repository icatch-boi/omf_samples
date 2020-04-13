//
// Created by wang.zhou on 2020/3/13.
//

#include "OmfRtmpClient.h"
#include "OmfAttrSet.h"
#include "OmfDbg.h"

#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntryGreen(s)

using namespace omf::rtmp;
using namespace omf::api;

static omf::api::OmfObject* _obj = nullptr;

OmfRtmpClient::OmfRtmpClient(const char* cfg) {

	const char* string = "type=Application,name=rtmpClient,serialize={"
							 "type=pipeline,name=plRtmp,serialize={"
								 "shm-source:name=shmPcm,connect={"
						 #if 0
									 "type=pipeline,name=plAac,serialize={"
										 "pv-pcm-src:name=mic"
										 "+PcmFilter:rate=%d"
										 "+aac-encoder:name=aac"
										 "+FifoSink:mame=afifo"
									 "}"
						 #else
						 			"type=Pipeline,name=plPreRecordAac"
						 #endif
								 "},sendmsg={preRecOutput={oper=AfterStart,send={forward}},preRecOutputDisable={oper=BeforeStop,send={forward}}}"
								 "+FifoFilter:name=fifoa,max=128"

								 "+shm-source:name=shmH264,connect={"
						 #if 0
									 "type=pipeline,name=plH264,serialize={"
										 "pv-yuv-src-adv:w=%d,h=%d"
										 "+FrameTrigger:name=trig,vbrc=%d"
										 "+h264-encoder-dma"
										 	":name=h264"
										 	",br=%d"
										 	",fr=%d"
										 	",gop=%d"
										 	",goptype=%s"
										 	",clock=400000"
										 "+FifoSink:name=vfifo"
									 "}"
						 #else
						 			"type=Pipeline,name=plPreRecordH264"
		  				 #endif
								 "},sendmsg={preRecOutput={oper=AfterStart,send={forward}},preRecOutputDisable={oper=BeforeStop,send={forward}}}"
		 						 ",link=none"
								 "+FifoFilter:name=fifov,max=60"

								 "+ele-sync:name=sync,link={fifoa,fifov}"
								 "+ele-rtmp-client:name=rtmpClient,url=%s"
							 "}"
						 "}";
	OmfAttrSet ap(cfg);
	auto rate = ap.GetOrDef("rate",0);dbgTestPVL(rate);
	auto w = ap.GetOrDef("w",0);dbgTestPVL(w);
	auto h = ap.GetOrDef("h",0);dbgTestPVL(h);
	auto br = ap.GetOrDef("br",0);dbgTestPVL(br);
	auto fr = ap.GetOrDef("fr",0);dbgTestPVL(fr);
	auto gop = ap.GetOrDef("gop",0);dbgTestPVL(gop);
	auto goptype = ap.Get("goptype");dbgTestPVL(goptype);
	auto url = ap.Get("url");dbgTestPVL(url);

	char buf[1024];
	sprintf(buf,string,/*rate,w,h,br,br,fr,gop,goptype,*/url);

	_obj = new OmfObject(buf);
}

OmfRtmpClient:: ~OmfRtmpClient(){
	if(_obj) delete _obj;
	_obj = nullptr;
}

OmfRtmpClient* OmfRtmpClient::CreateNew(const char *cfg, const char *url) {
	return new OmfRtmpClient(cfg);
}

bool OmfRtmpClient::Start() {
	if(_obj)
		return _obj->StatusUp("play");
	return true;
}

bool OmfRtmpClient::Stop() {
	if(_obj) {
		return _obj->StatusDown("null");
	}
	return true;
}