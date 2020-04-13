//
// Created by wang.zhou on 2020/2/25.
//
#if 0
#include "omf_api.h"
#include "omf_api_entry.h"
#include "stdlib.h"
#include <string>

int main(int argc,char** argv ){
	omfInit(0);
#if 0
	std::string param = "cmd=omfGetFromHost,sel=0xFFF6";
	omfCmd("shm0_cmd",param);

	printf("param_ret:%s\n",param.c_str());
#endif
	omfUninit(0);


	return 0;
}
#else

#include "omf_cmd_cxx.h"
#include "omf_api.h"
#include <thread>
#include <chrono>
#include <unistd.h>
#include "signal.h"
#include "stdlib.h"

extern "C" {
#include "omf_active.h"
}

///////////////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntryYellow(s)
///////////////////////////////////////////////////////
static void* _obj = 0;

static void handler(int signo){
	printf("signo(%d) begin!!!\n",signo);
	std::thread thread([signo](){
		///if timeout(5 seconds), exit force.
		std::this_thread::sleep_for(::std::chrono::milliseconds(5000));
		printf("signo(%d)  timeout!!!\n",signo);
		omfCommand("event_before_exit",0,0);
		printf("signo(%d)  timeout end!!!\n",signo);
		exit(0);
	});
	if(_obj){
		omfStatusDown(_obj,"null");
		omfDestroy(_obj);
	}
	_obj = 0;
	printf("signo(%d)  end!!!\n",signo);
	exit(0);
}

EXTERNC void* appOmfActivatePlugin() {
	unsigned sum = 0;

	ACTIVATE(Base);
	ACTIVATE(Core);

	/**< register plugin */
	ACTIVATE_PLUGIN(Base);
	ACTIVATE_PLUGIN(IO);
	ACTIVATE_PLUGIN(AV);

	///**< register plugin optional */
	ACTIVATE_PLUGIN(Ipc);
	ACTIVATE_PLUGIN(Shm);



	return (void*)sum;
}
int main(int argc,char** argv ){dbgTestPSL(Now()<<','<<argv[0]);auto tp0=Now();
	signal(SIGINT, handler);
	signal(SIGTERM, handler);

	appOmfActivatePlugin();
	omfInit(0);

	int duration = 3;
	if(argc>1) duration=atoi(argv[1]);
	const char* pattern =
			"type=Application,name=appH264,layout={"
				"type=Pipeline,name=plH264,layout={"
					"shm-source:connect={"
						"type=Pipeline,name=plPreRecordH264"
					"},sendmsg={preRecOutput={oper=AfterStart,send={forward}},preRecOutputDisable={oper=BeforeStop,send={forward}}}"
					"+RecordControler:name=ctrlH264,duration=%ds"
					"+IOSink:url=file:///tmp/preRec.h264,dbg=true,only1st=true"
				"}"
				//"+type=Pipeline,name=plAac,layout={"
					//"shm-source:connect={"
					//	"type=Pipeline,name=plPreRecordAac"
					//"},sendmsg={preRecOutput={oper=AfterStart,send={forward}},preRecOutputDisable={oper=BeforeStop,send={forward}}}"
					//"+RecordControler:name=ctrlAac,duration=6s"
					//"+IOSink:url=file:///tmp/preRec.aac,dbg=true""
				//"}"
			"}"
	;
	char str[1024];
	sprintf(str,pattern,duration);

	_obj = omfCreate(str);
	returnIfErrC(-1,!_obj);
	returnIfErrC(-1,!omfStatusUp(_obj,"play"));
	while(1){
		if(strcmp(omfStatusGet(_obj),"null") == 0)
			break;
		usleep(500000u);
	}
	returnIfErrC(-1,!omfDestroy(_obj));
	omfUninit(0);
	return 0;
}

#endif