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
#include "IMsgSite.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::api;
using namespace omf::chrono;
////////////////////////////////////////////
static int _id=0;
static int _target=0;
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
		{"id"		,'i', _id ,"set the message source id."},
		{"receive message"
		 "> demoMsgSite    	 		####receive message with target local site ID.  \n"
		 "> demoMsgSite -c    		####receive message with target local site ID by callback \n"
		 "> demoMsgSite -i 10   	####receive message with target 10#.  \n"
		 "> demoMsgSite -c -i 10    ####receive message with target 10# by callback \n"
  		 },
		{"cb"		,'c', [](){_cb=true;}, 	 "receive with register callback."},
		{"send message to target."
		 "> demoMsgSite -s -t 10 -m \"message context\"  		####send message to 10# \n"
		 "> demoMsgSite -s -i 20 -t 10 -m \"message context\"  	####send message from 20# to 10# \n"
   		},
		{"send"		,'s', [](){_send=true;}, "send message."},
		{"target"	,'t', _target ,"set the send target."},
		{"msg"		,'m', _msg ,"set the send message."},
		{"flags"	,'f', _flags ,"set the send message flags."},
		{},
};
////////////////////////////////////////////
static bool cbReceive(const void *data, int size, int sender, int target, unsigned flags){
	dbgTestPVL(_id);
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
static bool Process(OmfMain&omf,bool _dbg){
	//omf.Command("show_factory");
	auto site=IMsgSite::Globle();
	returnIfErrC(false,!site);
	returnIfErrC(false,!*site);
	dbgTestPVL(site->GetSiteID());
	if(_send){dbgTestPSL("send message["<<(void*)_flags<<']'<<_id<<"->"<<_target<<':'<<_msg<<',');
		int len = _msg?(strlen(_msg)+1):0;
		returnIfErrC(false,!site->Send(_id,_target,_msg,len,_flags));
	}else if(_cb){dbgTestPSL("receive "<<_id<<"# message by register cb");
		returnIfErrC(false,!site->Register(_id,&cbReceive));
		while(1)std::this_thread::sleep_for(1_s);
	}else{dbgTestPSL("receive "<<_id<<"# message");
		while(1){
			returnIfErrC(false,!site->Receive(_id,&cbReceive));
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
	dbgNotePSL("omfAacPlayer\n");
	///parse the input params
	OmfHelper helper(_options0,argc,argv);
	///--help
	returnIfTestC(0,!helper);
	///output the params list
	helper.Print();
	///check the params
	returnIfErrC(0,!Check());
	///
	OmfMain omf;
	omf.ShowModules();
	omf.Debug(helper.Debug());
	if(helper.Log())omf.LogConfig(helper.Log());
	///
	Process(omf,helper.Debug());
	///
	return 0;
}
