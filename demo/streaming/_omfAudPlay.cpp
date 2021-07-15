#include <thread>
#include <memory>
#include "_call.h"
#include "_hash.h"
#include "_chrono_base.h"
#include "OmfMain.h"
#include "OmfObject.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "OmfHelper.h"
#include "IAudioPlayer.h"
////////////////////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::chrono;
using namespace omf::api;
using namespace omf::api::streaming;
using namespace omf::api::streaming::common;
////////////////////////////////////////////////////////////
static const char* _fname=0;
static int _seconds=30;
static const char* _keywords="dualos-push";
static int _rate = 0;
static int _channels = 0;
static const char* _media=0;
static int _triggerInterval=100;//seconds
static int _packetSize = 0;
static bool _opus= false;
static Duration _packetDur = _duration_zero;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfAudPlay(...): \n"
		 "This demo shows how to play audio streaming using IAudioPlayer interface.\n"
		 "  omfAudPlay -n test.pcm -r 16000 -c 1 -m codec=pcm\n"
		 "  omfAudPlay -n test.alaw -r 8000 -c 1 -m codec=alaw\n"
		 "  omfAudPlay -n test.ulaw -r 8000 -c 1 -m codec=ulaw\n"
		 "  omfAudPlay -n test.g722 -r 16000 -c 1 -m codec=g722\n"
   		 "	omfAudPlay -n test.opus  -r 16000 -c 1 -m codec=opus\n"
		},
		{"fname",'n', _fname		,"play filename(*.aac)."},
		{"keywords",'k', _keywords	,"select the IAudioPlayer with keywords.Usually use the default values."},
		{"rate",'r', _rate			,"the input audio sample rate."},
		{"ch",'c', _channels		,"the input audio channels."},
		{"media",'m', _media		,"the input audio media info,such as \"g711:rate=48000,ch=1\"."},
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
		default:
			dbgTestPSL("unknow message:"<<msg);
			break;
	}
	return true;
}

static bool Process(OmfMain&omf){
	///////////////////////////////////////
	//create a IAudioPlayer instance with keywords.
	dbgTestPVL(_keywords);
	std::unique_ptr<IAudioPlayer> player(IAudioPlayer::CreateNew(_keywords));
	returnIfErrC(false,!player);
	//set audio player parameters
	player->SetSampleRate(_rate);
	player->SetChannels(_channels);
	player->SetMediaInfo(_media);
	player->EnableCodecOnLinux(true);
	//open streaming
	returnIfErrC(false,!player->ChangeUp(State::ready));
	//get streaming parameters after Open().
	auto info = player->GetAudioMediaInfo();
	dbgTestPVL(info.rate);
	dbgTestPVL(info.channels);
	dbgTestPVL(info.media);

	////////////////////////////////////////////////////////
	FILE* fd=fopen(_fname,"rb");
	if(!fd){
		dbgErrPSL("open file fail:"<<_fname);
		return false;
	}
	ExitCall ecfd([fd](){if(fd)fclose(fd);});
	//////////////////////////////////
	//streaming......
	auto duration = _packetDur;//ms
	int buf_size= _packetSize;
	std::unique_ptr<char[]> buf(new char[buf_size]);//malloc(buf_size);
	TimePoint tp=Now();
	dbgTestPVL(buf_size);
	dbgTestPVL(duration);
	///////////////////////////////////////////////////////
	returnIfErrC(false,!player->ChangeUp(State::play));
	dbgTestPSL("streaming ");
	while(!_exit){
		int data_len = buf_size;
		if(_opus){
			uint32 framesize = 0;
			uint32 ret = fread(&framesize,1,4,fd);
			if(ret<=0){
				dbgTestPSL("play end!");
				break;
			}
			buf_size = data_len = framesize;
		}
		data_len=fread(buf.get(),1,buf_size,fd);//printf("read file:%d\n",data_len);
		//dbgErrPSL(Hex(ftello64(fd)));
		if(data_len<=0){
			dbgTestPSL("play end!");
			break;
		}
		dbgTestPVL(data_len);
		///fill data
		returnIfErrCS(false,!player->IsSupportedPushFrame(),"this dev is not supported push frame!");
		std::shared_ptr<frame_t> frm{new frame_t{0,buf.get(),data_len,true,tp,nullptr}};
		returnIfErrC(false,!player->PushFrame(frm,true));
		///
		tp+=duration;
		//std::this_thread::sleep_until(tp);
		std::this_thread::sleep_for(10_ms);
	}
	////////////////////////////////////////////////////////
	return true;
}
////////////////////////////////////////////
static bool CheckPacketSize(){
	returnIfErrC(0,!_fname);
	///
	auto url = std::string("file://")+_fname;
	OmfAttrSet ap(url);
	returnIfErrC(false,!ap);
	auto ext = ap.Get("ext");
	returnIfErrC(false,!ext);
	///
	_packetDur = 100_ms;//ms
	_opus = false;
	auto ms = toMs(_packetDur);
	switch(::Hash(ext)){
		case ::Hash("pcm"):	
			_packetSize = _rate*_channels*2;
			break;
		case ::Hash("alaw"):
		case ::Hash("ulaw"):
			_packetSize = _rate*_channels*2/2;
			break;
		case ::Hash("g722"):
			_packetSize = _rate*_channels*2/4;
			break;
		case ::Hash("opus"):
			_packetDur = 20_ms;
			_packetSize = _rate;
			_opus = true;
			break;
		default:
			dbgErrPSL("unkonw audio file format:"<< _fname);
			return false;
	}
	_packetSize=_packetSize*ms/1000;
	return true;
}
static bool Check(){
	returnIfErrC(false,!_media);
	returnIfErrC(false,!CheckPacketSize());
	return true;
}

////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfAudPlay(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	//OmfMain::Globle().ShowClasses();
	returnIfErrC(0,!Check());
	///
	Process(OmfMain::Globle());
	///
	return 0;
}

