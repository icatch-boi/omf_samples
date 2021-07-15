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
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfMsgReceive(...): \n"
		 "This demo is used to show how to use OmfMsgSiteUser to receive messages.\n"
		 "  omfMsgReceive -n AAA   		####receive msg with the name \"AAA\".  \n"
  		 },
		{"name"	,'n', _name 		,"msg name."},
		{},
};
////////////////////////////////////////////
static bool cbReceive(const void *data, int size){
	dbgTestPSL("received msg("<<Now()<<"):"<<(const char*)data);
	return true;
}
static bool ReceiveWithRegisterCb(){
	OmfMain&omf=OmfMain::Globle();
	dbgTestPSL("register the cb on site to receive "<<_name<<"# message");
	returnIfErrC(false,!OmfMsgSiteUser::Register(_name,&cbReceive));
	dbgTestPSL("waiting ...");
	while(1){
		std::this_thread::sleep_for(1_s);
	}
	return true;
}
static bool ReceiveWithCb(){
	OmfMain&omf=OmfMain::Globle();
	dbgTestPSL("receive "<<_name<<"# message ...");
	while(1){
		returnIfErrC(false,!OmfMsgSiteUser::Receive(_name,&cbReceive));
		std::this_thread::sleep_for(1_s);
	}
	return true;
}
static bool Process(){
	ReceiveWithRegisterCb();
	//ReceiveWithCb();
	return true;
}
static bool Check(){
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("demoMsgReceive\n");
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
