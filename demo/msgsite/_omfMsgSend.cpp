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
#include "OmfMsgSiteUser.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::api;
using namespace omf::chrono;
////////////////////////////////////////////
static const char* _name=0;
static const char* _msg = 0;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfMsgSend(...): \n"
		 "This demo is used to show how to use OmfMsgSiteUser to send messages.\n"
		 "  omfMsgSend -n \"AAA\" -m \"message context\"  ####send msg with the name \"AAA\"  \n"
   		},
		{"msg"	,'m', _msg, "msg body."},
		{"name"	,'n', _name ,"msg name."},
		{},
};
////////////////////////////////////////////
static bool Process(){
	OmfMain&omf=OmfMain::Globle();
	bool _dbg=omf.DebugMode();
	///
	dbgTestPSL("send["<<_name<<"]:"<<_msg);
	int len = _msg?(strlen(_msg)+1):0;
	returnIfErrC(false,!OmfMsgSiteUser::Send(_name,_msg,len));
	return true;
}
static bool Check(){
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("demoMsgSiteUser\n");
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
