#include <thread>
#include <memory>
#include <vector>
#include <stdio.h>
#include "_call.h"
#include "_hash.h"
#include "_chrono_base.h"
#include "OmfMain.h"
#include "OmfObject.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "OmfHelper.h"
#include "IHlsSource.h"
////////////////////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::chrono;
using namespace omf::api;
using namespace omf::api::streaming;
using namespace omf::api::streaming::common;
using ISource = IStreamSource;
////////////////////////////////////////////////////////////
static const char* _fname=0;
static int _recordSeconds=30;
static int _tsSeconds=5;
////////////////////////////////////////////
static bool _exit = false;
static const char* _keywords = "dualos-prerec";
static const char* _tspattern = "REC_%04u-%02u-%02u_%02u-%02u-%02u.ts";
static const char* _m3u8pattern = "REC_%04u-%02u-%02u_%02u-%02u-%02u.m3u8";
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfHlsSource(...): \n"
		 "This demo shows how to demux the file using IDemuxer interface.\n"
		 "> omfHlsSource -d 30 -t 5 \n"
		},
		{"duration"	,'d', _recordSeconds,"set the record duration."},
		{"tsduration"	,'t', _tsSeconds,"set the record duration."},
		{},
};
////////////////////////////////////////////
static bool MessageProcess(const char* msg0){
	dbgTestPSL(msg0);
	OmfAttrSet ap(msg0);
	returnIfErrC(true,!ap);
	auto msg = ap.Get("msg");
	returnIfErrC(true,!msg);
	switch(Hash(msg)){
		case Hash("error"):
		case Hash("err"):
			dbgNotePSL("error");
			_exit=true;
			break;
		case Hash("eos"):
			dbgNotePSL("eos");
			_exit=true;
			break;
		case Hash("open"):
			dbgNotePSL("open");
			break;
		case Hash("close"):
			_exit=true;
			dbgNotePSL("close");
			break;
		case Hash("m3u8"):
			{
				printf("msg m3u8file \n");
				auto m3u8 = ap.Get("filename");
				_exit = true;
				dbgNotePSL("m3u8file");
			}
			break;
		case Hash("ts"):
			{
				printf("msg ts \n");
				auto filename = ap.Get("filename");
				dbgNotePSL("ts");
			}
			break;
		default:
			dbgErrPSL("unknow message:"<<msg);
			break;
	}
	return true;
}

static bool Process(){
	///////////////////////////////////////
	///create a IDemuxer instance with keywords.
	dbgTestPVL(_keywords);
	std::unique_ptr<IHlsSource> hlsSrc(IHlsSource::CreateNew(_keywords));
	returnIfErrC(false,!hlsSrc);
	///set audio player parameters
	hlsSrc->SetAudioCodec("aac:br=128000");
	hlsSrc->SetTsDuration(_tsSeconds);
	hlsSrc->SetTsUrlPattern(_tspattern);
	hlsSrc->SetM3u8UrlPattern(_m3u8pattern);
	hlsSrc->SetFilePath("/mnt/diskd/omf/bin");
	hlsSrc->EnablePrerec(1);
	hlsSrc->RegisterMessageCallback(&MessageProcess);
	//demux->Url(((std::string)"file://"+_fname).c_str());
	///open streaming
	returnIfErrC(false,!hlsSrc->ChangeUp(State::ready));
	///////////////////////////////////////////////////////
	returnIfErrC(false,!hlsSrc->ChangeUp(State::play));
	int cnt = _recordSeconds;
	while(cnt>0){
		std::this_thread::sleep_for(1000_ms);
		cnt--;
	}
	returnIfErrC(false,!hlsSrc->ChangeDown(State::ready));
	while(!_exit){
		std::this_thread::sleep_for(10_ms);
	}
	returnIfErrC(false,!hlsSrc->ChangeDown(State::null));
	////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////
	return true;
}
////////////////////////////////////////////
static bool Check(){
	//returnIfErrC(false,!_media);
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfHlsSource(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	//OmfMain::Globle().ShowClasses();
	returnIfErrC(0,!Check());
	///
	Process();
	///
	return 0;
}

