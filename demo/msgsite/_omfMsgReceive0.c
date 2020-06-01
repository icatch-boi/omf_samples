//
// Created by jg.wang on 2020/2/25.
//
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "omf_api.h"
#include "omf_msg_site_user.h"
////////////////////////////////////////////
#define dbgTestPL() printf("%s/%d:%s\n",__FILE__,__LINE__,__FUNCTION__)
#define dbgTestPSL(v) printf("%s/%d:%s#%s\n",__FILE__,__LINE__,__FUNCTION__,v)
#define dbgTestPVL0(v) printf("%s/%d:%s#%s=%s\n",__FILE__,__LINE__,__FUNCTION__,#v,v)
#define dbgTestPVL(v) printf("%s/%d:%s#%s=%d\n",__FILE__,__LINE__,__FUNCTION__,#v,v)
#define returnIfErrC(v,c) if(c){dbgTestPSL(#c);return v;}
#define bool int
#define false 0
#define true 1
/////////////////////////////////////////////
static const char* _name=0;
static const char* _log="all=false,err=true,note=true";
static bool _show=false;
////////////////////////////////////////////
static char _short_options[]="n:H:L:?:S";
static struct option _long_options[]={
		{"help",0,0,'H'},
		{"show",0,0,'S'},
		{"log",1,0,'L'},
		{"name",1,0,'n'},
		{0,0,0,0}
};
static void OptionHelper(){
	printf("demoMsgReceive0(...): \n");
	printf("This demo is used to show how to use IMsgSite to send or receive messages.\n");
	printf("   demoMsgReceive0 -n AAA                  ####receive message of AAA .  \n");
	printf("--log,-L[%s]     log config.\n",_log);
	printf("--show,-S[%d]     show omf info.\n",_show);
	printf("--name,-n[%s]     receiver.\n",_name);
}
static int OptionParse(int argc,char **argv){
	int opt;
	//int digit_optind = 0;
	int option_index = 0;
	while((opt =getopt_long(argc,argv,_short_options,_long_options,&option_index))!= -1)
	{
		//printf("opt = %c\t\t", opt);
		//printf("optarg = %s\t\t",optarg);
		//printf("optind = %d\t\t",optind);
		//printf("argv[optind] =%s\t\t", argv[optind]);
		//printf("option_index = %d\n",option_index);
		///
		switch(opt) {
			case -1:
				return 1;
			case '?':
			case 'H':
				OptionHelper();
				return 0;
			case 'L':
				_log = optarg;
				break;
			case 'S':
				_show = true;
				break;
			case 'n':
				_name=optarg;
				break;
			default:
				printf("undefine flags:%c\n",(char)opt);
				break;
		}
	}
	return 1;
}

////////////////////////////////////////////
static bool _received=false;
static int cbReceive(void*hd,const void *data, int size){
	printf("%s/%d:%s#received %s msg:%s\n",__FILE__,__LINE__,__FUNCTION__,_name,(const char*)data);
	return 1;
}
static bool receiveMsg(){
	printf("receive message:%s\n",_name);
	while(1){
		returnIfErrC(false,!omfMsgSiteUserReceive(_name,&cbReceive,0));
		usleep(100000u);
	}
	return true;
}
static bool receiveMsgByRegisterCb(){
	printf("receive %s# message by register cb\n",_name);
	returnIfErrC(false,!omfMsgSiteUserRegister(_name,&cbReceive,0));
	while(1){
		usleep(1000000u);
	}
	return true;
}
static int Process(){
	//return receiveMsg();
	return receiveMsgByRegisterCb();
}
static int Print(){
	dbgTestPVL0(_name);

	return 1;
}
static int Check(){
	return 1;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgTestPSL("demo0MsgSite\n");
	///parse the input params
	if(!OptionParse(argc,argv))return 0;
	///output the params list
	Print();
	///check the params
	returnIfErrC(0,!Check());
	///
	omfInit(0);
	omfCommand("show_modules",0,0);
	if(_show){
		omfCommand("show_classes",0,0);
		omfCommand("show_factory",0,0);
	}
	omfCommand("sigsegv", 0, 0);
	omfCommand("dbgEn",_log,0);
	///
	Process();
	///
	return 0;
}
