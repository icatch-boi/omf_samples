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
#include "IDemuxer.h"
#include "IH264Source.h"
#include "IAacSource.h"
#include "IDemuxer.h"
#include "OmfRtmpClient.h"
////////////////////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::chrono;
using namespace omf::api;
using namespace omf::api::streaming;
using namespace omf::api::streaming::common;
using namespace omf::rtmp;
using ISource = IStreamOutput;
////////////////////////////////////////////////////////////
static const char* _fname=0;
static FILE* _fd=0;
////////////////////////////////////////////
static bool _exit = false;
static bool _dumpFrm = true;
static const char* _keywords = "dualos-pull";
////////////////////////////////////////////
static const char* _url = "rtmp://192.168.0.147:1935/live/Vi37";
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfDemuxer(...): \n"
		 "This demo shows how to demux the file using IDemuxer interface.\n"
		 "  omfDemuxer -n test.mp4 -u rtmp://x.x.x.x/ \n"
		},
		{"fname",'n', _fname		,"demux filename(*.mp4)."},
		{"url"	,'u', _url			,"rtmp server url."},
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
		default:
			dbgTestPSL("unknow message:"<<msg);
			break;
	}
	return true;
}
static bool ProcessAac(ISource*src) {
	auto aac = src->AsType<IAacSource>();
	if (!aac)return false;
	auto info = aac->GetAacMediaInfo();
	dbgTestPVL(info.rate);
	dbgTestPVL(info.channels);
	dbgTestPVL(info.hasADTS);
	dbgTestPVL(info.profile);
	dbgTestPVL(info.version);
	dbgTestPVL(info.rateidx);
	dbgTestPVL(info.adtsLength);
	dbgTestPVL(info.bitrate);
	return true;
}
static bool ProcessH264(ISource*src){
	auto h264=src->AsType<IH264Source>();
	if(!h264)return false;
	auto info=h264->GetH264MediaInfo();
	dbgTestPVL(info.width);
	dbgTestPVL(info.height);
	dbgTestPVL(info.gop);
	dbgTestPVL(info.gopType);
	dbgTestPVL(info.sps);
	dbgTestPVL(info.spsLength);
	dbgTestPVL(info.pps);
	dbgTestPVL(info.ppsLength);
	dbgTestPVL(info.iframe.spsOffset);
	dbgTestPVL(info.iframe.spsLenght);
	dbgTestPVL(info.iframe.ppsOffset);
	dbgTestPVL(info.iframe.ppsLenght);
	dbgTestPVL(info.iframe.dataOffset);
	dbgTestPVL(info.pframe.dataOffset);
	dbgTestPVL((void*)info.extraData);
	dbgTestPVL(info.extraSize);
	dbgTestDL(info.extraData,info.extraSize);
	dbgTestPL();
	return true;
}

static bool ProcessFrame(std::shared_ptr<frame_t> frm,FILE*fd,int line){
	if(!frm->data || !frm->size)
		return false;

	///write to file
	if(fd)fwrite(frm->data,1,frm->size,fd);
	///
	if(!_dumpFrm)
		return true;
	dbgTestPS(line<<'#'<<frm->index
	              << ',' << frm->data
	              << ',' << frm->size
	              << ',' << frm->iskeyframe
	              << ',' << frm->pts
	              << ','
	);
	dbgTestDL(frm->data, 16);
	return true;
}
static bool Process(OmfMain&omf){
	///////////////////////////////////////
	//create a IDemuxer instance with keywords.
	dbgTestPVL(_keywords);
	std::unique_ptr<IDemuxer> demux(IDemuxer::CreateNew(_keywords));
	returnIfErrC(false,!demux);
	//set audio player parameters
	demux->RegisterMessageCallback(&MessageProcess);
	demux->Url(((std::string)"file://"+_fname).c_str());

	std::unique_ptr<OmfRtmpClient> rtmp{OmfRtmpClient::CreateNew("",_url)};
	returnIfErrC(false,!rtmp.get());
	returnIfErrC(false,!rtmp->SetSourceObject((void*)demux.get()));
	returnIfErrC(false,!rtmp->Start());

	while(!_exit){
		std::this_thread::sleep_for(100_ms);
	}
	returnIfErrC(false,!demux->ChangeDown(State::null));
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
	dbgNotePSL("omfDemuxer(...)\n");
	///parse the input params
	OmfHelper helper(_options0,argc,argv);
	///--help
	returnIfTestC(0,!helper);
	///check the params
	//returnIfErrC(0,!Check());
	///init the omf module
	OmfMain omf;
	//omf.Command("show_classes",0,0);
	omf.LogConfig("all=true,err=true,note=true");
	omf.Helper(helper);
	///process
	Process(omf);
	///
	return 0;
}

