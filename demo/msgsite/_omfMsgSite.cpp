//
// Created by jg.wang on 2020/2/25.
//
#include <thread>
#include "_hash.h"
#include "_chrono_base.h"
#include "OmfMain.h"
#include "OmfObject.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "OmfHelper.h"
#include "OmfMsgSite.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::api;
using namespace omf::chrono;
////////////////////////////////////////////
static int _srcID=0;
static int _msgID=0;
static const char* _msg=0;
static unsigned _flags=0;
static bool _send = false;
static bool _cb = false;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"demoMsgSite(...): \n"
		 "This demo is used to show how to use IMsgSite to send or receive messages.\n"
		},
		{"\nreceive message"
		 "> demoMsgSite    	 		####receive message with local site ID.  \n"
		 "> demoMsgSite -c    		####receive message with local site ID by callback \n"
		 "> demoMsgSite -t 10   	####receive 10# message.  \n"
		 "> demoMsgSite -c -t 10    ####receive 10# message by callback \n"
  		 },
		{"\nsend message."
		 "> demoMsgSite -s -t 10 -m \"message context\"  		####send 10# message  \n"
		 "> demoMsgSite -s -i 20 -t 10 -m \"message context\"  	####send 10# message from 20# user \n"
		},
		{"srcID"	,'i'	, _srcID 	,"set the message source id."},
		{"cb"		,'c'	, [](){_cb=true;}	, 	 "receive with register callback."},
		{"send"		,'s'	, [](){_send=true;}	, "send message."},
		{"msgID"	,'t'	, _msgID 	,"set the message ID."},
		{"msg"		,'m'	, _msg 		,"set the message body."},
		{"flags"	,'f'	, _flags 	,"set the message flags."},
		{},
};
////////////////////////////////////////////
static bool cbReceive(const void *data, int size, int sender, int target, unsigned flags){
	dbgTestPVL(_srcID);
	//dbgTestPVL(data);
	//dbgTestPVL(size);
	dbgTestPVL(Now());
	dbgTestDL(data,size);
	dbgTestPVL(sender);
	dbgTestPVL(target);
	dbgTestPVL(flags);
	if(data){
		dbgTestPVL((const char*)data);
	}
	return true;
}
static bool Process(){
	OmfMain&omf=OmfMain::Globle();
	bool _dbg=omf.DebugMode();
	//omf.Command("show_factory");
	auto site=OmfMsgSite::Globle();
	returnIfErrC(false,!site);
	returnIfErrC(false,!*site);
	dbgTestPVL(site->GetSiteID());
	if(_send){
		dbgTestPSL("send "<<_msgID<<"# message["<<(void*)_flags<<']'<<_srcID<<"#:"<<_msg);
		int len = _msg?(strlen(_msg)+1):0;
		returnIfErrC(false,!site->Send(_srcID,_msgID,_msg,len,_flags));
	}else if(_cb){
		dbgTestPSL("register the cb on site to receive "<<_msgID<<"# message");
		returnIfErrC(false,!site->Register(_msgID,&cbReceive));
		while(1)std::this_thread::sleep_for(1_s);
	}else{
		dbgTestPSL("receive "<<_msgID<<"# message");
		while(1){
			returnIfErrC(false,!site->Receive(_msgID,&cbReceive));
			std::this_thread::sleep_for(100_ms);
		}
	}
	return true;
}
static bool Check(){
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("demoMsgSite\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	returnIfErrC(0,!Check());
	///
	Process();
	///
	return 0;
}
