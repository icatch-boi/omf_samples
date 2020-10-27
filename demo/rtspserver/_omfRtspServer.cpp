#include "OmfMain.h"
#include "OmfHelper.h"
#include "OmfDbg.h"
#include "OmfRtspService.h"
#include "_chrono_base.h"
#include <string>
#include <thread>
#include <chrono>
#include <memory>
///////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
//////////////////////////////////////
using namespace omf;
using namespace omf::chrono;
using namespace omf::rtsp;
using namespace omf::api;
//////////////////////////////////////
static const char* _maps=0;
static OmfHelper::Item _options0[]{
		{"omfRtspServer(...): \n"
		 "this demo is to start a RTSP server. \n"
            "omfRtspServer \n"
		    "omfRtspServer -m \"1536x1536={codec=h264,w=1536,h=1536,br=1000000,fr=30,gop=30+codec=aac,ch=1,br=128000,bw=16000},720x720={codec=h264,w=720,h=720,br=500000,fr=30,gop=30+codec=aac,ch=1,br=128000,bw=16000}\"\n"
		},
		{"maps",'m', _maps		,"add key-url pairs to maps ."},
		{},
};
using namespace omf::api;
//////////////////////////////////////
static bool RtspServer(const char*urlmap,const char*authmap) {//dbgSkyPL();
	dbgTestPVL(urlmap);
	dbgTestPVL(authmap);
	std::unique_ptr<OmfRtspService> rtsp{OmfRtspService::CreateNew(urlmap,authmap)};
	returnIfErrC(false,!rtsp.get());
	returnIfErrC(false,!rtsp->Start());dbgTestPSL(Now());
	while(1){std::this_thread::sleep_for(std::chrono::hours(1ll));dbgTestPL();}
	returnIfErrC(false,!rtsp->Stop());dbgTestPL();
	return true;
}

static bool CreateRtspUrlMap(char* buff){
	///h264 width,height,bitrate,framerate,gop
	const char* vattern="%s={codec=h264,w=%d,h=%d,br=%d,fr=%d,gop=%d},";
	///aac bitrate,bandwidth
	const char* aattern="%s={codec=aac,ch=%d,br=%d,bw=%d},";
	const char* pattern="%s={codec=h264,w=%d,h=%d,br=%d,fr=%d,gop=%d+codec=aac,ch=%d,br=%d,bw=%d},";
	char* ptr=buff;
	/////////////////////////
	ptr+=sprintf(ptr,pattern,"stream0",/*h264*/1920,1080,1024*128*8 ,24, 24, /*aac*/1,38000,16000);
	ptr+=sprintf(ptr,pattern,"stream1",/*h264*/1280, 720,1024*128*2 ,24, 24, /*aac*/1,38000,16000);
	ptr+=sprintf(ptr,pattern,"stream2",/*h264*/640 , 360,1024*128*1 ,24, 24, /*aac*/1,38000,16000);
	ptr+=sprintf(ptr,pattern,"fhd-aac",/*h264*/1920,1080,1024*128*8 ,24, 24, /*aac*/1,38000,16000);
	ptr+=sprintf(ptr,pattern,"hd-aac", /*h264*/1280, 720,1024*128*2 ,24, 24, /*aac*/1,38000,16000);
	ptr+=sprintf(ptr,pattern,"vga-aac",/*h264*/640 , 360,1024*128*1 ,24, 24, /*aac*/1,38000,16000);
	ptr+=sprintf(ptr,vattern,"fhd",/*h264*/1920,1080,1024*128*8 ,24, 24);
	ptr+=sprintf(ptr,vattern,"hd",/*h264*/1920,1080,1024*128*4 ,24, 24);
	ptr+=sprintf(ptr,vattern,"vga",/*h264*/1920,1080,1024*128*1 ,24, 24);
	ptr+=sprintf(ptr,aattern,"aac",/*aac*/1,38000,16000);
	///add
	ptr+=sprintf(ptr,pattern,"1536x1536",/*h264*/1536,1536,1024*128*12 ,24, 24, /*aac*/1,38000,16000);
	ptr+=sprintf(ptr,pattern,"720x720",/*h264*/720,720,1024*128*4 ,24, 24, /*aac*/1,38000,16000);
	///
	if(_maps){
		dbgNotePSL("custom key-urls:\n"<<_maps<<'\n');
		ptr+=sprintf(ptr,"%s",_maps);
	}
	///
	*ptr=0;
	return true;
}

static bool CreateRtspAuthMap(char* buff){
	///eg..
	/// "<auth0>=<keywords0>,<auth1>=<keywords1>"
	buff[0]=0;
	return true;
}

int main(int argc, char *argv[]){
	dbgNotePSL("omfRtspServer");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///
	std::unique_ptr<char[]> urlmap{new char[1024*32]};
	CreateRtspUrlMap(urlmap.get());

	std::unique_ptr<char[]> authmap{new char[1024*32]};
	CreateRtspAuthMap(authmap.get());

	RtspServer(urlmap.get(),authmap.get());
	///
	return 0;
}

