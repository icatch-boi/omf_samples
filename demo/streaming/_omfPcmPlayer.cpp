#include <cstdio>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <unistd.h>
#include "omf_api.h"
#include "OmfMain.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "IPcmPlayer.h"
////////////////////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf::api;
using namespace omf::api::streaming;
using namespace omf::api::streaming::common;
////////////////////////////////////////////////////////////
int main(int argc,char* argv[]){
	printf("param([filename] [rate] [channel])\n");
	if(argc<=0)return 0;
	int rate=16000;
	int channels=1;
	const char* fn="test16000_1.pcm";
	if(argc>1) fn=argv[1];
	if(argc>2) rate=atoi(argv[2]);
	if(argc>3) channels=atoi(argv[3]);
	printf("filename:%s rate:%d channel:%d\n",fn,rate,channels);
	//////////////////////////////////////////////////////
	OmfMain omf;
	omf.ShowModules();
	omf.Debug(false);
	omf.LogConfig("all=false,err=true,note=true");
	///////////////////////////////////////////////////////
	IPcmPlayer* player = IPcmPlayer::CreateNew("dualos-push");
	returnIfErrC(-1,!player);
	player->SetSampleRate(rate);
	player->SetChannels(channels);

	////////////////////////////////////////////////////////
	int vol=50;
	player->SelectSpeaker("dualos");//select the Speaker.
	if(player->Speaker()->IsSupportVolumeControl())player->Speaker()->Volume(vol);
	////////////////////////////////////////////////////////
	returnIfErrC(false,!player->ChangeUp(State::play));
	////////////////////////////////////////////////////////
	//streaming......
	FILE* fd = fopen(fn,"rb");printf("open file:%d\n",fd);
	if(!fd){
		printf("can't open file:%s,%d\n",fn,fd);
		return 0;
	}
	///
	int buf_size=1024*channels*2;
	char *buf = new char[buf_size];//malloc(buf_size);
	int duration = (buf_size*1000)/(rate*channels*16/8);//ms
	printf("buf_size:%d,duartion:%dms\n",buf_size,duration);
	///////////////////////////////////////////////////////
	while(1){
		int data_len = buf_size;
		data_len=fread(buf,1,buf_size,fd);//printf("read file:%d\n",data_len);
		if(data_len<=0){
			break;
		}
		///fill data
		returnIfErrCS(-1,!player->IsSupportedPushFrame(),"this dev is not supported push frame!");
		std::shared_ptr<frame_t> frm{new frame_t{0,buf,data_len,true,std::chrono::steady_clock::now(),nullptr}};
		returnIfErrCS(-1,!player->PushFrame(frm),"push frame fail!");
		usleep(duration*1000u);

	}
	////////////////////////////////////////////////////////
	delete[] buf;
	if(fd)fclose(fd);
	////////////////////////////////////////////////////////
	//stop streaming
	returnIfErrC(false,!player->ChangeDown(State::ready));
	//close streaming
	returnIfErrC(false,!player->ChangeDown(State::null));
	delete player;
	return 0;
}
