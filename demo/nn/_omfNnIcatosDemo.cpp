#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "omf_api.h"
#include "omf_msg_site_user.h"

typedef enum{
	DATA_INTERACTION_OPERATION_SET = 0,
	DATA_INTERACTION_OPERATION_GET = 1,
	DATA_INTERACTION_OPERATION_CMD = 2,
}DataInteractionOperation_e;

#define dbgTestPL() printf("%s\%d:%s:\n",__FILE__,__LINE__,__FUNCTION__);
#define dbgTestPSL(v) printf("%s\%d:%s:%s\n",__FILE__,__LINE__,__FUNCTION__,v);
#define dbgTestPDL(v) printf("%s\%d:%s:%s=%d\n",__FILE__,__LINE__,__FUNCTION__,#v,v);
#define dbgTestPVL(v) printf("%s\%d:%s:%s=%s\n",__FILE__,__LINE__,__FUNCTION__,#v,v);
#define dbgTestPPL(v) printf("%s\%d:%s:%s=%p\n",__FILE__,__LINE__,__FUNCTION__,#v,v);
#define returnIfErrC(v,c) if(c){dbgTestPSL(#c);return v;}
#define returnIfErrC0(c) if(c){dbgTestPSL(#c);return;}

char input[64];		

static int cbReceive(void*hd,const void *data, int size){
	printf("%s/%d:%s#received msg:%s\n",__FILE__,__LINE__,__FUNCTION__,(const char*)data);	
	return 1;
}

static bool receiveMsgByRegisterCb(){	
	returnIfErrC(false,!omfMsgSiteUserRegister("nnresult",&cbReceive,0));
	while(1){
		usleep(1000000u);
	}
	return true;
}

static int Process(){	
	printf("run nn demo msg process\n");
	return receiveMsgByRegisterCb();
}

static void nndemo_exit(void)
{
	printf("nn demo stop\n");
  sprintf(input, "cmd=nn,operation=%d,model=0,msgen=false", DATA_INTERACTION_OPERATION_SET);
  returnIfErrC(0,!omfCommand("shm0_cmd",input, 0));	
  
  exit(0);
}

static void nndemo_init(void)
{	
	/* regester signal handler */
	struct sigaction action;
  action.sa_handler = nndemo_exit;  
  sigemptyset(&action.sa_mask);  
  action.sa_flags = 0;  
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGKILL, &action, NULL);
  sigaction(SIGQUIT, &action, NULL);
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGSTOP, &action, NULL);
    		
	omfInit(0);
	
	//show the modules loaded.
	omfCommand("show_modules",0,0);
	
	//disable all debug log.
	omfCommand("dbgEn","all=false,err=true,note=true",0);
	
	//model - wait for nn - NOT READY
	sprintf(input, "cmd=nn,operation=%d,model=1,msgen=true", DATA_INTERACTION_OPERATION_SET);	
	returnIfErrC(0,!omfCommand("shm0_cmd",input, 0));	
	
	printf("nn demo start\n");
	Process();
}

int main(){	
  nndemo_init();
	return 0;
}