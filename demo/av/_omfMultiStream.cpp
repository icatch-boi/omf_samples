//
// Created by jg.wang on 2020/8/25.
//

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
#include "IH264Source.h"
#include "IAacSource.h"
#include "IAudioSource.h"

////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::chrono;
using namespace omf::api;
using namespace omf::api::streaming;
using namespace omf::api::streaming::common;
////////////////////////////////////////////////////////////
static int _seconds=30;//seconds
static bool _dumpFrm=false;//seconds
static bool _saveFile=false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfMultiStream(...): \n"
		 "This demo shows how to use multi-streaming \n"
		 "  omfMultiStream -d5 \n"
		},
		{"duration"	,'d', _seconds	,"record duration(*s)."},
		{"Frame"	,'F', [](){_dumpFrm=true;}	,"dump frame info."},
		{"Save"	    ,'S', [](){_saveFile=true;}	,"save frame to file."},
		{},
};

////////////////////////////////////////////////////////////
class CItem{
public:
	CItem(IStreamSource*,const char*fn);
	~CItem();

protected:
	std::string _fname;
	FILE*_fd=0;
	std::shared_ptr<IStreamSource> _src;
	const char* _error=0;
	TimePoint _1st=_timepoint_min;
	Duration _duration=_duration_zero;
	int _frameCount=0;
	int _totalSize=0;
public:
	bool MessageProcess(const char* msg0);
	bool ProcessFrame(std::shared_ptr<frame_t>& frm);

	bool StartStream();
	bool StopStream();

	bool OpenFile();
	bool CloseFile();
};
////////////////////////////////////////////
CItem::CItem(IStreamSource*src,const char*fn)
	:_src(src)
	,_fname(fn)
{

}
CItem::~CItem(){

}
bool CItem::OpenFile(){
	returnIfTestC(true,_fd);
	returnIfErrC(false,_fname.empty());
	returnIfTestC(false,!_saveFile);
	_fd = fopen(_fname.c_str(), "wb");
	returnIfErrCS(false,!_fd,"open file fail:" << _fname);
	return true;
}
bool CItem::CloseFile(){
	if(_fd)fclose(_fd);_fd=0;
	return true;
}
bool CItem::StartStream(){
	returnIfErrC(false,!_src);
	return _src->ChangeUp(OmfState::State::play);
}
bool CItem::StopStream(){
	returnIfErrC(false,!_src);
	returnIfErrC(false,!_src->ChangeDown(OmfState::State::null));
	///
	dbgNotePVL(_fname);
	dbgNotePVL(_1st);
	dbgNotePVL(_duration);
	dbgNotePVL(_frameCount);
	dbgNotePVL(_totalSize);
	return true;
}
bool CItem::MessageProcess(const char* msg0){
	dbgTestPSL(msg0);
	OmfAttrSet ap(msg0);
	returnIfErrC(true,!ap);
	auto msg = ap.Get("msg");
	returnIfErrC(true,!msg);
	switch(Hash(msg)){
		case Hash("error"):
		case Hash("err"):
			dbgErrPSL("error");
			_error="msg error";
			break;
		case Hash("warning"):
			dbgNotePSL("warning");
			break;
		default:
			dbgTestPSL("unknow message:"<<msg);
			break;
	}
	return true;
}

bool CItem::ProcessFrame(std::shared_ptr<frame_t>& frm) {//dbgTestPSL((void *) this);
	if (!frm || !frm->data || !frm->size) {//dbgTestPSL((void *) this);
		return false;
	}
	if (_error) {//dbgTestPSL((void *) this<<':'<<_error);
		return false;
	}
	///
	if(_1st==_timepoint_min)_1st=frm->pts;
	_duration=frm->pts-_1st;
	_frameCount++;
	_totalSize+=frm->size;
	///
	if(_dumpFrm) {//dbgTestPL();
		dbgTestPS( this->_fname<<'#'<<frm->index
		               << ',' << frm->pts
		               << ',' << frm->data
		               << ',' << frm->size
		               << ',' << frm->iskeyframe
		);
		dbgTestDL(frm->data, 16);
	}
	///write to file
	if(_fd)fwrite(frm->data,1,frm->size,_fd);//dbgTestPSL((void*)this);

	return true;
}
static std::shared_ptr<CItem> CreateItem(IStreamSource*src,const char*fn){
	returnIfErrC(nullptr,!src);
	//
	auto item=new CItem(src,fn);
	returnIfErrC(nullptr,!item);
	//
	src->RegisterMessageCallback([item](const char*msg){
		return item->MessageProcess(msg);
	});
	src->RegisterOutputCallback([item](std::shared_ptr<frame_t>&frm){
		return item->ProcessFrame(frm);
	});
	return std::shared_ptr<CItem>(item);
}
static std::shared_ptr<CItem> CreateItemH264PreRecord(){
	//bool _dbg=OmfMain::Globle().DebugMode();
	///////////////////////////////////////
	//create a h264Source instance with keywords.
	auto src =IH264Source::CreateNew("prerec-push");
	returnIfErrC(nullptr,!src);
	//
	auto item = CreateItem(src,"rec.h264");
	returnIfErrC(nullptr,!item);
	//set PreRecord h264
	src->Set("prerec=1");
	//open streaming
	returnIfErrC(nullptr,!src->ChangeUp(State::ready));
	//get streaming parameters after Open().
	auto info = src->GetH264MediaInfo();
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

	return item;
}
static std::shared_ptr<CItem> CreateItemH264(){
	//bool _dbg=OmfMain::Globle().DebugMode();
	///////////////////////////////////////
	//create a h264Source instance with keywords.
	auto src =IH264Source::CreateNew("vbrc-vfrc-push");
	returnIfErrC(nullptr,!src);
	//
	auto item = CreateItem(src,"pv.h264");
	returnIfErrC(nullptr,!item);
	//set yuv srouce parameters
	src->SetWidth(1920);
	src->SetHeight(1080);
	//set h264 encoder parameters
	src->SetGop(150);
	src->SetGopType("ippp");
	//set BitRateControl
	src->SetBitRate(1000000);
	src->SetFrameRate(15);
	//open streaming
	returnIfErrC(nullptr,!src->ChangeUp(State::ready));
	//get streaming parameters after Open().
	auto info = src->GetH264MediaInfo();
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

	return item;
}
static std::shared_ptr<CItem> CreateItemAacPreRecord(){
	//bool _dbg=OmfMain::Globle().DebugMode();
	///////////////////////////////////////
	//create a AacSource instance with keywords.
	auto src = IAacSource::CreateNew("prerec-push");
	returnIfErrC(nullptr,!src);
	//
	auto item = CreateItem(src,"rec.aac");
	returnIfErrC(nullptr,!item);
	///set aac codec
	//set PreRecord aac
	src->Set("prerec=1");
	//open streaming
	returnIfErrC(nullptr,!src->ChangeUp(State::ready));
	//get streaming parameters after Open().
	auto info = src->GetAacMediaInfo();
	dbgTestPVL(info.rate);
	dbgTestPVL(info.channels);
	dbgTestPVL(info.hasADTS);
	dbgTestPVL(info.profile);
	dbgTestPVL(info.version);
	dbgTestPVL(info.rateidx);
	dbgTestPVL(info.adtsLength);
	dbgTestPVL(info.bitrate);

	return item;
}
static std::shared_ptr<CItem> CreateItemG722(){dbgTestPL();
	//bool _dbg=OmfMain::Globle().DebugMode();
	///////////////////////////////////////
	//create a IAudioSource instance with keywords.
	auto src = IAudioSource::CreateNew("push");
	returnIfErrC(nullptr,!src);
	//
	auto item = CreateItem(src,"pv.g722");
	returnIfErrC(nullptr,!item);
	//set audio srouce parameters
	src->SetCodec("g722");
	//open streaming
	returnIfErrC(nullptr,!src->ChangeUp(State::ready));
	//get streaming parameters after Open().
	auto info = src->GetAudioMediaInfo();
	dbgTestPVL(info.rate);
	dbgTestPVL(info.channels);
	dbgTestPVL(info.media);

	return item;
}
////////////////////////////////

int main(int argc,char* argv[]){
	dbgNotePSL("omfMultiStream(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///create demo instance
	dbgTestPL();
	std::shared_ptr<CItem> items[]{
			CreateItemH264()
			,CreateItemG722()
			,CreateItemH264PreRecord()
			,CreateItemAacPreRecord()

	};
	///open files
	dbgTestPL();
	for(auto&item:items){
		item->OpenFile();
	}
	///start
	dbgTestPL();
	for(auto&item:items){
		item->StartStream();
	}
	///wait
	dbgTestPL();
	std::this_thread::sleep_for(std::chrono::seconds(_seconds));
	///stop
	dbgTestPL();
	for(auto&item:items){
		item->StopStream();
	}
	///close files
	dbgTestPL();
	for(auto&item:items){
		item->CloseFile();
	}
	///
	dbgTestPL();
	return 0;
}





