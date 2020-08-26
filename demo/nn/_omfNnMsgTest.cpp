#include "omf_api.h"
#include "omf_msg_site_user.h"
#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#define returnIfErrC(v,c) if(c){printf("%s/%d:%s:%s\n",__FILE__,__LINE__,__FUNCTION__,#c);return v;}
#define dbgTestPL() printf("%s\%d:%s:\n",__FILE__,__LINE__,__FUNCTION__);
#define dbgTestPSL(v) printf("%s\%d:%s:%s\n",__FILE__,__LINE__,__FUNCTION__,v);
#define dbgTestPDL(v) printf("%s\%d:%s:%s=%d\n",__FILE__,__LINE__,__FUNCTION__,#v,v);
#define dbgTestPVL(v) printf("%s\%d:%s:%s=%s\n",__FILE__,__LINE__,__FUNCTION__,#v,v);
#define returnIfErrC(v,c) if(c){dbgTestPSL(#c);return v;}
#define returnIfErrC0(c) if(c){dbgTestPSL(#c);return;}

static const char* _msgName="NNResult";

static int MsgcbReceive(void* priv_hd, const void *data, int size){
	dbgTestPDL((unsigned)priv_hd);
	dbgTestPDL((unsigned)data);
	dbgTestPDL(size);
	if(data){
		dbgTestPVL((const char*)data);
	}
	return 1;
}
static int MsgProcess() {
	returnIfErrC(0, !omfMsgSiteUserRegister(_msgName, &MsgcbReceive, 0));
	while (1)usleep(100000u);
	return 1;
}
static int MsgClose() {
	returnIfErrC(0,!omfMsgSiteUserUnRegister(_msgName));
	return 1;
}

int main(int argc, char *argv[]){
	omfInit(0);

	MsgProcess();


	MsgClose();

	omfUninit(0);
	return 0;
}