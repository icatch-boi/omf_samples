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
/**h264**/
static int _width = 1920;
static int _height = 1080;
static int _framerate_max = 30;
static int _framerate_min = 10;
static int _v_bitrate_max =1000;///kb
static int _v_bitrate_min = 500;///kb
static const char* _goptype = "ippp";
static int _gop = 15;
/**aac**/
static int _a_bitrate = 128;///KB
static int _channel = 1;
//static int _samplerate = 16000;
/**rtmp**/
static int _seconds = 3600;
static const char* _url = "rtmp://192.168.0.147:1935/live/Vi37";
//////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"OmfRtmpClient(...): \n"
		 "This demo shows how to push rtmp steam to server. \n"
		 "> OmfRtmpClient -d30 -w1920 -h1080 -f30 -b 128 -t ippp -g30 \n"
		 "> OmfRtmpClient -d30 -w1280 -h720  -f30 -b 128 -t ippp -g30\n"
		},
		{"the yuv paramers:"},
		{"w"		,'w'		, _width			,"width of image."},
		{"h"		,'h'		, _height			,"height of image."},
		{"frmax"	,'F'		, _framerate_max	,"frame max number per seconds."},
		{"frmin"	,'f'		, _framerate_min	,"frame min number per seconds."},
		{"the h264 paramers:"},
		{"vbrmax"	,'B'		, _v_bitrate_max	,"bitrate(kb) max of h264 encoder."},
		{"vbrmin"	,'b'		, _v_bitrate_min	,"bitrate(kb) min of h264 encoder."},
		{"goptype"	,'t'		, _goptype			,"gop type(iiii,ippp,ibbp) of h264 encoder."},
		{"gop"		,'g'		, _gop				,"gop of h264 encoder."},
		{"the aac paramers:"},
//		{"rate"		,'r'		, _samplerate		,"samplereate of pcm."},
		{"abr"		,'a'		, _a_bitrate		,"set the audio bitrate(kb) to aac codec."},
		{"channel"  ,'c'		, _channel			,"set the target channels to aac codec.0:follow the channels of pcm src,other is force to rechannels."},
		{"the rtmp paramers:"},
		{"duration"	,'d'		, _seconds			,"rtmp stream duration(*s)."},
		{"url"		,'u'		, _url				,"rtmp server url."},
		{},
};
static bool RtmpServer(const char* cfgmap,const char* url) {//dbgSkyPL();
	dbgTestPVL(cfgmap);
	std::unique_ptr<OmfRtmpClient> rtmp{OmfRtmpClient::CreateNew(cfgmap,url)};
	returnIfErrC(false,!rtmp.get());
	returnIfErrC(false,!rtmp->Start());dbgTestPL();
	std::this_thread::sleep_for(std::chrono::seconds(_seconds));dbgTestPL();
	returnIfErrC(false,!rtmp->Stop());dbgTestPL();

	return true;
}

static bool CreateRtmpUrlMap(char* buff){

	sprintf(buff,"w=%d,h=%d,frmax=%d,frmin=%d,"
			     "vbrmax=%d,vbrmin=%d,goptype=%s,gop=%d,"
		         "abitrate=%d,channel=%d",
				 _width,_height,_framerate_max,_framerate_min,
			     _v_bitrate_max,_v_bitrate_min,_goptype,_gop,
			     _a_bitrate,_channel);

	return true;
}

int main(int argc, char *argv[]){
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///
	std::unique_ptr<char[]> cfgmap{new char[1024*32]};
	CreateRtmpUrlMap(cfgmap.get());

	RtmpServer(cfgmap.get(),_url);


	return 0;
}