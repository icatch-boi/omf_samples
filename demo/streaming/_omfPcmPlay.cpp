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
#include "IPcmPlayer.h"
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
static int _triggerInterval=100;//seconds
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfPcmPlay(...): \n"
		 "This demo shows how to play PCM streaming using IPcmPlayer interface.\n"
		 "> omfPcmPlay -n test.pcm -r 16000 -c 1\n"
		},
		{"fname",'n', _fname		,"play filename(*.aac)."},
		{"keywords",'k', _keywords	,"select the IPcmPlayer with keywords.Usually use the default values."},
		{"rate",'r', _rate			,"the input audio sample rate."},
		{"ch",'c', _channels		,"the input audio channels."},
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
static bool Process(bool _dbg){
	///////////////////////////////////////
	//create a IPcmPlayer instance with keywords.
	dbgTestPVL(_keywords);
	std::unique_ptr<IPcmPlayer> player(IPcmPlayer::CreateNew(_keywords));
	returnIfErrC(false,!player);
	//set pcm player parameters
	player->SetSampleRate(_rate);
	player->SetChannels(_channels);

	//open streaming
	returnIfErrC(false,!player->ChangeUp(State::ready));
	//get streaming parameters after Open().
	auto info = player->GetPcmMediaInfo();
	dbgTestPVL(info.rate);
	dbgTestPVL(info.channels);
	dbgTestPVL(info.isSigned);
	dbgTestPVL(info.bitWidth);
	dbgTestPVL(info.byteWidth());
	dbgTestPVL(info.bytePerSample());

	////////////////////////////////////////////////////////
	FILE* fd=fopen(_fname,"rb");
	if(!fd){
		dbgErrPSL("open file fail:"<<_fname);
		return false;
	}
	ExitCall ecfd([fd](){if(fd)fclose(fd);});
	//////////////////////////////////
	//volume......
	//ProcessSpeaker(player.get());
	//////////////////////////////////
	//streaming......
	auto duration = 1000_ms;//ms
	int buf_size=_rate*_channels*2;
	std::unique_ptr<char[]> buf(new char[buf_size]);//malloc(buf_size);
	TimePoint tp=Now();
	dbgTestPVL(buf_size);
	dbgTestPVL(duration);
	///////////////////////////////////////////////////////
	returnIfErrC(false,!player->ChangeUp(State::play));
	dbgTestPSL("streaming ");
	while(!_exit){
		int data_len = buf_size;
		data_len=fread(buf.get(),1,buf_size,fd);//printf("read file:%d\n",data_len);
		if(data_len<=0){
			dbgTestPSL("play end!");
			break;
		}
		dbgTestPVL(data_len);
		///fill data
		returnIfErrCS(false,!player->IsSupportedPushFrame(),"this dev is not supported push frame!");
		std::shared_ptr<frame_t> frm{new frame_t{0,buf.get(),data_len,true,tp,nullptr}};
		returnIfErrC(false,!player->PushFrame(frm));
		///
		tp+=duration;
		std::this_thread::sleep_until(tp);
	}
	////////////////////////////////////////////////////////
	return true;
}
////////////////////////////////////////////
static bool Check(){
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfPcmSrc(...)\n");
	///parse the input params
	OmfHelper helper(_options0,argc,argv);
	///--help
	returnIfTestC(0,!helper);
	///output the params list
	helper.Print();
	///check the params
	returnIfErrC(0,!Check());
	///init the omf module
	OmfMain omf;
	omf.ShowModules();
	omf.Debug(helper.Debug());
	if(helper.Log())omf.LogConfig(helper.Log());
	///process
	Process(helper.Debug());
	///
	return 0;
}

