
#define REMOTE_PUSH_PRINT_EN  1
#define REMOTE_PUSH_DBG_DEF_LEVEL		REMOTE_PUSH_LVL_DEBUG



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "omf_msg_site_user.h"
#include "omf_api.h"

#include "tinyaiot_data_def.h"
#include "remote_push_interface.h"
#include "remote_push_errno.h"
#include "remote_push_print.h"

#define DESTNATION_FILLENAME		PUSH_MSG_KEY_TIMEINSECS_VAL".jpg"
#define SOURCE_FILETYPE			TINYAIOT_FILE_TYPE_MSG
#define FILE_READ_BUF_SIZE	   (1024*1024)

#define LINUX_PUSH_MSG_ID			"pushMsgViaLinux"
#define LINUX_PUSH_FILE_ID			"pushFileViaLinux"

/*************************************************/


/************************************************/

remotePushErrno_e remotePushDemo(int argc, char **argv)
{
	if(argc<1) {
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"input: remotepush --help for usage\n");
		return REMOTE_PUSH_ERRNO_SUCCESS;
	}
	if(0 == strcmp("--help", argv[0])) {
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"Usage:\n");
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remotepush file [fileName]:Upload a file to remote-app.\ne.g:remotepush file B:\\test.jpg\n\n");
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remotepush msg [msgID]:push a message specified by msgID to remote-app.\ne.g: remotepush msg 100\n\n");
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[100]:PIR.\n");
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[102]:Low Battery.\n");
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[106]:OTA downlaod file from server succeed.\n");
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[107]:OTA downlaod file from server failed.\n");
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[108]:OTA install file from server succeed.\n");
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[109]:OTA install file from server failed.\n");
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[110]:Exit Preview.\n");
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[301]:ring.\n");
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[900]:The package has been delivered.\n");
		return REMOTE_PUSH_ERRNO_SUCCESS;
	}
	remotePushModuleInit();
	if(0 == strcmp("msg", argv[0])) {
		if(argc < 2) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"Usage:\n");
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remotepush msg [msgID]:push a message specified by msgID to remote-app.\n e.g: remotepush msg 100\n\n");
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[100]:PIR.\n");
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[102]:Low Battery.\n");
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[106]:OTA downlaod file from server succeed.\n");
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[107]:OTA downlaod file from server failed.\n");
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[108]:OTA install file from server succeed.\n");
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[109]:OTA install file from server failed.\n");
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[110]:Exit Preview.\n");
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[301]:ring.\n");
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"msgID[900]:The package has been delivered.\n");
			return REMOTE_PUSH_ERRNO_SUCCESS;
		}
		if(0 == strcmp("100", argv[1])) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remote push message, ID[100]\n");
			remotePushMsgBody_t msgBody;
			memset(&msgBody, 0, sizeof(remotePushMsgBody_t));
			msgBody.baseParam.type = MSG_EVT_MOTION_DET;
			msgBody.baseParam.data = "alion";
			msgBody.baseParam.size = strlen("alion");
			strcpy(msgBody.baseParam.timeStr, "2020-03-24 11:42:50");
			msgBody.msgCenterParam.timeStamp = 12345678;
			msgBody.msgCenterParam.coordinationGrpNum = 2;
			msgBody.msgCenterParam.coordinate[0].left = 50;
			msgBody.msgCenterParam.coordinate[0].top = 50;
			msgBody.msgCenterParam.coordinate[0].width = 150;
			msgBody.msgCenterParam.coordinate[0].height = 150;
			msgBody.msgCenterParam.coordinate[1].left = 200;
			msgBody.msgCenterParam.coordinate[1].top = 200;
			msgBody.msgCenterParam.coordinate[1].width = 100;
			msgBody.msgCenterParam.coordinate[1].height = 100;
			remotePushModuleMsgPushTrigger(&msgBody);
		}
		else if(0 == strcmp("900", argv[1])) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remote push message, ID[900]\n");
			remotePushMsgBody_t msgBody;
			memset(&msgBody, 0, sizeof(remotePushMsgBody_t));
			msgBody.baseParam.type = MSG_EVT_USER_PACKAGE_DETECT;
			msgBody.baseParam.data = "The package has been delivered.";
			msgBody.baseParam.size = strlen("The package has been delivered.");
			strcpy(msgBody.baseParam.timeStr, "2020-04-16 16:58:10");
			msgBody.msgCenterParam.timeStamp = 12345678;
			msgBody.msgCenterParam.coordinationGrpNum = 1;
			msgBody.msgCenterParam.coordinate[0].left = 50;
			msgBody.msgCenterParam.coordinate[0].top = 50;
			msgBody.msgCenterParam.coordinate[0].width = 150;
			msgBody.msgCenterParam.coordinate[0].height = 150;
			remotePushModuleMsgPushTrigger(&msgBody);
		}
		else if(0 == strcmp("102", argv[1])) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remote push message, ID[102]\n");
			remotePushMsgBody_t msgBody;
			memset(&msgBody, 0, sizeof(remotePushMsgBody_t));
			msgBody.baseParam.type = MSG_EVT_LOW_BAT;
			msgBody.baseParam.data = "Low Battery";
			msgBody.baseParam.size = strlen("Low Battery");
			strcpy(msgBody.baseParam.timeStr, "2020-04-16 16:31:30");
			remotePushModuleMsgPushTrigger(&msgBody);
		}
		else if(0 == strcmp("110", argv[1])) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remote push message, ID[110]\n");
			remotePushMsgBody_t msgBody;
			memset(&msgBody, 0, sizeof(remotePushMsgBody_t));
			msgBody.baseParam.type = MSG_EVT_EXITPV;
			msgBody.baseParam.data = "Exit Preview";
			msgBody.baseParam.size = strlen("Exit Preview");
			strcpy(msgBody.baseParam.timeStr, "2020-04-16 17:11:25");
			remotePushModuleMsgPushTrigger(&msgBody);
		}
		else if(0 == strcmp("106", argv[1])) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remote push message, ID[106]\n");
			remotePushMsgBody_t msgBody;
			memset(&msgBody, 0, sizeof(remotePushMsgBody_t));
			msgBody.baseParam.type = MSG_EVT_OTA_DOWNLOAD_DONE;
			msgBody.baseParam.data = "OTA downlaod file from server succeed";
			msgBody.baseParam.size = strlen("OTA downlaod file from server succeed");
			strcpy(msgBody.baseParam.timeStr, "2020-04-16 17:49:25");
			remotePushModuleMsgPushTrigger(&msgBody);
		}
		else if(0 == strcmp("107", argv[1])) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remote push message, ID[107]\n");
			remotePushMsgBody_t msgBody;
			memset(&msgBody, 0, sizeof(remotePushMsgBody_t));
			msgBody.baseParam.type = MSG_EVT_OTA_DOWNLOAD_ERR;
			msgBody.baseParam.data = "OTA downlaod file from server failed";
			msgBody.baseParam.size = strlen("OTA downlaod file from server failed");
			strcpy(msgBody.baseParam.timeStr, "2020-04-16 17:49:25");
			remotePushModuleMsgPushTrigger(&msgBody);
		}
		else if(0 == strcmp("108", argv[1])) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remote push message, ID[108]\n");
			remotePushMsgBody_t msgBody;
			memset(&msgBody, 0, sizeof(remotePushMsgBody_t));
			msgBody.baseParam.type = MSG_EVT_OTA_INSTALL_DONE;
			msgBody.baseParam.data = "OTA install file from server succeed";
			msgBody.baseParam.size = strlen("OTA install file from server succeed");
			strcpy(msgBody.baseParam.timeStr, "2020-04-16 17:49:25");
			remotePushModuleMsgPushTrigger(&msgBody);
		}
		else if(0 == strcmp("109", argv[1])) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remote push message, ID[109]\n");
			remotePushMsgBody_t msgBody;
			memset(&msgBody, 0, sizeof(remotePushMsgBody_t));
			msgBody.baseParam.type = MSG_EVT_OTA_INSTALL_ERR;
			msgBody.baseParam.data = "OTA install file from server failed";
			msgBody.baseParam.size = strlen("OTA install file from server failed");
			strcpy(msgBody.baseParam.timeStr, "2020-04-16 17:49:25");
			remotePushModuleMsgPushTrigger(&msgBody);
		}
		else if(0 == strcmp("301", argv[1])) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remote push message, ID[301]\n");
			remotePushMsgBody_t msgBody;
			memset(&msgBody, 0, sizeof(remotePushMsgBody_t));
			msgBody.baseParam.type = MSG_EVT_RING;
			msgBody.baseParam.data = "ring message";
			msgBody.baseParam.size = strlen("ring message");
			strcpy(msgBody.baseParam.timeStr, "2020-04-17 10:14:35");
			msgBody.msgCenterParam.timeStamp = 12345678;
			msgBody.faceRecogParam.recogResult = 1;
			msgBody.faceRecogParam.faceIDNum = 2;
			msgBody.faceRecogParam.faceID[0] = 1001;
			msgBody.faceRecogParam.faceID[1] = 1002;
			msgBody.msgCenterParam.coordinationGrpNum = 2;
			msgBody.msgCenterParam.coordinate[0].left = 50;
			msgBody.msgCenterParam.coordinate[0].top = 50;
			msgBody.msgCenterParam.coordinate[0].width = 150;
			msgBody.msgCenterParam.coordinate[0].height = 150;
			msgBody.msgCenterParam.coordinate[1].left = 200;
			msgBody.msgCenterParam.coordinate[1].top = 200;
			msgBody.msgCenterParam.coordinate[1].width = 100;
			msgBody.msgCenterParam.coordinate[1].height = 100;
			remotePushModuleMsgPushTrigger(&msgBody);
		}
		else {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"unknown command\n");
			return REMOTE_PUSH_ERRNO_PARAM;
		}
	}
	else if(0 == strcmp("file", argv[0])) {
		if(argc < 2) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"Usage:\n");
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remotepush file [fileName]:Upload a file to remote-app.\ne.g:remotepush file B:\\test.jpg\n\n");
			return REMOTE_PUSH_ERRNO_SUCCESS;
		}
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"remote push file(%s)\n", argv[1]);
		FILE *fp = fopen((const char*)argv[1], "r+");
		if(NULL == fp) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"[%s-%d] file(%s) open fail", __FUNCTION__,__LINE__, argv[1]);
			return REMOTE_PUSH_ERRNO_OTHERS;
		}
		unsigned char* readBuf = (unsigned char*)malloc(FILE_READ_BUF_SIZE);
		if(!readBuf) {
			fclose(fp);
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"[%s-%d] malloc fail", __FUNCTION__,__LINE__);
			return REMOTE_PUSH_ERRNO_OTHERS;
		}
		unsigned int actualReadedSize = fread(readBuf, 1, FILE_READ_BUF_SIZE, fp);
		if(actualReadedSize<0) {
			fclose(fp);
			free(readBuf);
			dbgLogPrint(REMOTE_PUSH_LVL_INFO,"[%s-%d] file(%s) read fail", __FUNCTION__,__LINE__, argv[1]);
			return REMOTE_PUSH_ERRNO_OTHERS;
		}
		fclose(fp);
		remotePushFileBody_t fileInfo;
		memset(&fileInfo, 0, sizeof(remotePushFileBody_t));
		fileInfo.data = readBuf;
		fileInfo.size = actualReadedSize;
		fileInfo.dataType = TINYAIOT_FILE_TYPE_MSG;
		strcpy(fileInfo.destName, "12345678.jpg");
		strcpy(fileInfo.timeStr, "2020-03-24 11:42:50");
		remotePushModuleFilePushTrigger(&fileInfo);
		free(readBuf);
	}
	else {
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"Error:operation illegal!\n");
	}
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

/*
 *Name: msgPushProcCB
 *Brief:callback function to deal the message that from RTOS to inform LINUX to push message
 *Args:data --- data from RTOS, size --- size of data
 *Return:int (0--fail, 1---success)
 */

int msgPushProcCB(void* priv_hd, const void *data, int size)
{
	int iRet = 1;
	if(NULL == data) {
		iRet = 0;
		return iRet;
	}
	remotePushMsgBody_t *pMsgBody = (remotePushMsgBody_t *)malloc(size);
	if(NULL == pMsgBody) {
		iRet = 0;
		return iRet;
	}
	memcpy(pMsgBody, data, size);
	
	if(REMOTE_PUSH_ERRNO_SUCCESS != remotePushModuleMsgPushTrigger(pMsgBody)) {
		iRet = 0;
	}
	free(pMsgBody);
	iRet = 1;
	return iRet;
}

/*
 *Name: filePushProcCB
 *Brief:callback function to deal the message that from RTOS to inform LINUX to push file
 *Args:data --- data from RTOS, size --- size of data
 *Return:int (0--fail, 1---success)
 */

int filePushProcCB(void* priv_hd, const void *data, int size)
{
	printf("[%s:%d]pushing file\n", __FUNCTION__, __LINE__);
	
	int iRet = 1;
	if(NULL == data) {
		iRet = 0;
		return iRet;
	}

	remotePushFileBody_t *pFileBody = (remotePushFileBody_t *)malloc(sizeof(remotePushFileBody_t));
	if(NULL == pFileBody) {
		iRet = 0;
		return iRet;
	}
	
	memcpy(pFileBody, data, sizeof(remotePushFileBody_t));
	
	char *pFileData = (char*)malloc(pFileBody->size);
	if(NULL == pFileData) {
		iRet = 0;
		return iRet;
	}
	memcpy(pFileData, pFileBody->data, pFileBody->size);
	pFileBody->data = pFileData;
	
	if(REMOTE_PUSH_ERRNO_SUCCESS != remotePushModuleFilePushTrigger(pFileBody)) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]pushing file fail", __FUNCTION__, __LINE__);
		iRet = 0;
	}
	free(pFileBody->data);
	free(pFileBody);
	iRet = 1;
	return iRet;
}


int main(int argc, char **argv)
{
	omfInit(0);
	remotePushModuleInit();
	omfMsgSiteUserRegister(LINUX_PUSH_MSG_ID, msgPushProcCB, NULL);
	omfMsgSiteUserRegister(LINUX_PUSH_FILE_ID, filePushProcCB, NULL);
	remotePushDemo(argc-1, ++argv);
	while(1) {
		usleep(1000*1000);
	}
	omfUninit(0);
	return 0;
}
