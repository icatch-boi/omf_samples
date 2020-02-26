//
// Created by jg.wang on 2020/2/25.
//
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omf_api.h"
#include "omf_msg_site.h"
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
static int _id=0;
static int _target=0;
static const char* _msg="";
static unsigned _flags=0;
static int _send = 0;
static int _cb = 0;
////////////////////////////////////////////
static char _short_options[]="i:c:s:t:m:f:H:?";
static struct option _long_options[]={
		{"help",1,0,'H'},
		{"id",1,0,'i'},
		{"cb",0,0,'c'},
		{"send",0,0,'s'},
		{"target",1,0,'t'},
		{"msg",1,0,'m'},
		{"flags",1,0,'f'},
		{0,0,0,0}
};
static void OptionHelper(){
	printf("demoMsgSite(...): \n");
	printf("This demo is used to show how to use IMsgSite to send or receive messages.\n");

	printf("--id,-i[%d]         set the message source id.\n",_id);
	printf("receive message\n");
	printf(" > demoMsgSite    	   ####receive message with target local site ID.  \n");
	printf(" > demoMsgSite -c       ####receive message with target local site ID by callback \n");
	printf(" > demoMsgSite -i 10    ####receive message with target 10#.  \n");
	printf(" > demoMsgSite -c -i 10 ####receive message with target 10# by callback \n");

	printf("--cb,-c[%d]         receive with register callback.\n",_cb);
	printf("send message to target.\n");
	printf(" > demoMsgSite -s -t 10 -m \"message context\"  		####send message to 10# \n");
	printf(" > demoMsgSite -s -i 20 -t 10 -m \"message context\"  	####send message from 20# to 10# \n");

	printf("--send,-s[%d]       send message.\n",_send);
	printf("--target,-t[%d]     set the send target.\n",_target);
	printf("--msg,-m[%s] 		set the send message.\n",_msg);
	printf("--flags,-f[%d] 		set the send message flags.\n",_flags);
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
			case 'i':
				_id=atoi(optarg);
				break;
			case 'c':
				_cb=1;
				break;
			case 's':
				_send=1;
				break;
			case 't':
				_target=atoi(optarg);
				break;
			case 'm':
				_msg=optarg;
				break;
			case 'f':
				_flags=atoi(optarg);
				break;
			default:
				printf("undefine flags:%c\n",(char)opt);
				break;
		}
	}
	return 1;
}

////////////////////////////////////////////
static int cbReceive(void*hd,const void *data, int size, int sender, int target, unsigned flags){
	dbgTestPVL(_id);
	dbgTestPVL((unsigned)data);
	dbgTestPVL(size);
	dbgTestPVL(sender);
	dbgTestPVL(target);
	dbgTestPVL(flags);
	if(data){
		dbgTestPVL0((const char*)data);
	}
	return 1;
}
static int Process(){
	void* site=omfMsgSite0Get();
	returnIfErrC(false,!site);
	returnIfErrC(false,!omfMsgSiteIsWorking(site));
	dbgTestPVL(omfMsgSiteGetID(site));
	if(_send){
		printf("send message[%08x]%d->%d:%s\n",_flags,_id,_target,_msg);
		int len = _msg?(strlen(_msg)+1):0;
		returnIfErrC(false,!omfMsgSiteSend1(site,_id,_target,_msg,len,_flags));
	}else if(_cb){
		printf("receive %d # message by register cb\n",_id);
		returnIfErrC(false,!omfMsgSiteRegister1(site,_id,&cbReceive,0));
		while(1)usleep(1000000u);
	}else{
		printf("receive %d# message\n",_id);
		while(1){
			returnIfErrC(false,!omfMsgSiteReceive1(site,_id,&cbReceive,0));
			usleep(100000u);
		}
	}
	return true;
}
static int Print(){
	dbgTestPVL(_id);
	dbgTestPVL(_target);
	dbgTestPVL(_send);
	dbgTestPVL(_cb);
	dbgTestPVL(_flags);
	if(_msg)dbgTestPVL0(_msg);
	return 1;
}
static int Check(){
	return 1;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgTestPSL("omfAacPlayer\n");
	///parse the input params
	if(!OptionParse(argc,argv))return 0;
	///output the params list
	Print();
	///check the params
	returnIfErrC(0,!Check());
	///
	omfInit(0);
	omfCommand("show_classes",0,0);
	omfCommand("sigsegv", 0, 0);
	omfCommand("dbgEn","all=false,err=true,note=true",0);
	///
	Process();
	///
	return 0;
}
