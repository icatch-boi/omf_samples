//
// Created by jg.wang on 2020/2/14.
//
#include "OmfMain.h"
#include "OmfDbg.h"
#include "OmfHelper.h"
#include "OmfRtmpClient.h"
#include <string>
#include <thread>
///////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
//////////////////////////////////////
using namespace omf;
using namespace omf::rtmp;
using namespace omf::api;
//////////////////////////////////////
static int _seconds = 3600;//seconds
static int _samplerate = 16000;
static int _width = 1920;
static int _height = 1080;
static int _framerate = 30;
static int _bitrate = 500000;///kb
static const char* _goptype = "ippp";
static int _gop = 15;
static const char* _url = "rtmps://live-api-s.facebook.com:443/rtmp/978265025900961?s_bl=1&s_ps=1&s_sw=0&s_vt=api-s&a=Abxccekoz3DYFWyF";
//////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"OmfRtmpClient(...): \n"
		 "This demo shows how to push rtmp steam to server. \n"
		 "> OmfRtmpClient -d30 -w1920 -h1080 -f30 -b 128 -t ippp -g30 \n"
		 "> OmfRtmpClient -d30 -w1280 -h720  -f30 -b 128 -t ippp -g30\n"
		},
		{"the yuv paramers:"},
		{"w"		,'w', _width	,"width of image."},
		{"h"		,'h', _height	,"height of image."},
		{"fr"		,'f', _framerate,"frame number per seconds."},
		{"the pcm paramers:"},
		{"rate"		,'w', _samplerate	,"samplereate of pcm."},
		{"the h264 paramers:"},
		{"bitrate"	,'b', _bitrate	,"bitrate(kb) of h264 encoder."},
		{"goptype"	,'t', _goptype	,"gop type(iiii,ippp,ibbp) of h264 encoder."},
		{"gop"		,'g', _gop		,"gop of h264 encoder."},
		{"misc:"},
		{"the rtmp paramers:"},
		{"duration"	,'d', _seconds	,"rtmp stream duration(*s)."},
		{"url"		,'u', _url		,"rtmp server url."},
		{},
};
static bool RtmpClient(const char*urlmap) {//dbgSkyPL();
	dbgTestPVL(urlmap);
	std::unique_ptr<OmfRtmpClient> rtmp{OmfRtmpClient::CreateNew(urlmap,urlmap)};
	returnIfErrC(false,!rtmp.get());
	returnIfErrC(false,!rtmp->Start());dbgTestPL();
	std::this_thread::sleep_for(std::chrono::seconds(_seconds));dbgTestPL();
	returnIfErrC(false,!rtmp->Stop());dbgTestPL();

	return true;
}

static bool CreateRtmpUrlMap(char* buff){

	sprintf(buff,"rate=%d,w=%d,h=%d,br=%d,fr=%d,gop=%d,goptype=%s,url=%s"
			    ,_samplerate
				,_width
				,_height
				,_bitrate
				,_framerate
				,_gop
				,_goptype
				,_url);

	return true;
}

int main(int argc, char *argv[]){
	///parse the input params
	OmfHelper helper(_options0,argc,argv);
	///--help
	returnIfTestC(0,!helper);
	///output the params list
	helper.Print();
	///init the omf module
	OmfMain omf;
	///show the modules loaded.
	omf.ShowModules();
	///disable all debug log.
	omf.LogConfig("all=true,err=true,note=true");
	///debug
	omf.Debug(false);
	///
	std::unique_ptr<char[]> urlmap{new char[1024*32]};
	CreateRtmpUrlMap(urlmap.get());

	RtmpClient(urlmap.get());


	return 0;
}