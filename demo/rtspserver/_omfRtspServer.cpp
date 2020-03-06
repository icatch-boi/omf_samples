#include "OmfMain.h"
#include "OmfDbg.h"
#include "OmfRtspService.h"
#include <string>
#include <thread>
#include <chrono>
#include <memory>
///////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
//////////////////////////////////////
using namespace omf::rtsp;
using namespace omf::api;
//////////////////////////////////////
static bool RtspServer(const char*urlmap,const char*authmap) {//dbgSkyPL();
	dbgTestPVL(urlmap);
	dbgTestPVL(authmap);
	std::unique_ptr<OmfRtspService> rtsp{OmfRtspService::CreateNew(urlmap,authmap)};
	returnIfErrC(false,!rtsp.get());
	returnIfErrC(false,!rtsp->Start());dbgTestPL();
	while(1){std::this_thread::sleep_for(std::chrono::hours(1ll));dbgTestPL();}
	returnIfErrC(false,!rtsp->Stop());dbgTestPL();
	return true;
}

static bool CreateRtspUrlMap(char* buff){
	///h264 width,height,bitrate,framerate,gop
	const char* vattern="%s={h264={w=%d,h=%d,br=%d,fr=%d,gop=%d}}";
	///aac bitrate,bandwidth
	const char* aattern="%s={aac={rate=%d,ch=%d,br=%d,bw=%d}}";
	const char* pattern="%s={h264={w=%d,h=%d,br=%d,fr=%d,gop=%d},aac={rate=%d,ch=%d,br=%d,bw=%d}}";
	char* ptr=buff;
	/////////////////////////
	ptr+=sprintf(ptr,pattern,"stream0",/*h264*/1920,1080,1024*128*8 ,24, 24, /*aac*/16000,1,38000,16000);*ptr++=',';
	ptr+=sprintf(ptr,pattern,"stream1",/*h264*/1280, 720,1024*128*2 ,24, 24, /*aac*/16000,1,38000,16000);*ptr++=',';
	ptr+=sprintf(ptr,pattern,"stream2",/*h264*/640 , 360,1024*128*1 ,24, 24, /*aac*/16000,1,38000,16000);*ptr++=',';
	ptr+=sprintf(ptr,pattern,"fhd-aac",/*h264*/1920,1080,1024*128*8 ,24, 24, /*aac*/16000,1,38000,16000);*ptr++=',';
	ptr+=sprintf(ptr,pattern,"hd-aac", /*h264*/1280, 720,1024*128*2 ,24, 24, /*aac*/16000,1,38000,16000);*ptr++=',';
	ptr+=sprintf(ptr,pattern,"vga-aac",/*h264*/640 , 360,1024*128*1 ,24, 24, /*aac*/16000,1,38000,16000);*ptr++=',';
	ptr+=sprintf(ptr,vattern,"fhd",/*h264*/1920,1080,1024*128*8 ,24, 24);*ptr++=',';
	ptr+=sprintf(ptr,vattern,"hd",/*h264*/1920,1080,1024*128*4 ,24, 24);*ptr++=',';
	ptr+=sprintf(ptr,vattern,"vga",/*h264*/1920,1080,1024*128*1 ,24, 24);*ptr++=',';
	ptr+=sprintf(ptr,aattern,"aac",/*aac*/16000,1,38000,16000);//*ptr++=',';
	*ptr++=0;

	return true;
}

static bool CreateRtspAuthMap(char* buff){
	///eg..
	/// "<auth0>=<keywords0>,<auth1>=<keywords1>"
	buff[0]=0;
	return true;
}

int main(int argc, char *argv[]){
	OmfMain omf;
	///show the modules loaded.
	omf.ShowModules();
	///disable all debug log.
	//omf.LogConfig("all=false,err=true,note=true");
	///debug
	omf.Debug(true);
	///process signal int
	omf.SignalINT(true);
	///
	std::unique_ptr<char[]> urlmap{new char[1024*32]};
	CreateRtspUrlMap(urlmap.get());

	std::unique_ptr<char[]> authmap{new char[1024*32]};
	CreateRtspAuthMap(authmap.get());

	RtspServer(urlmap.get(),authmap.get());
	///
	return 0;
}

