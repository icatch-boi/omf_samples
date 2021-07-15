/*
 *File Name:remote_push_io.c
 *Brief:
 *Date:2020-08-05
 */

#define REMOTE_PUSH_PRINT_EN  1
#define REMOTE_PUSH_DBG_DEF_LEVEL		REMOTE_PUSH_LVL_INFO

/**************************************************************************
 *                           I N C L U D E S	                           *
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tinyaiot_data_def.h"
#include "tinyaiot_interface.h"
#include "tinyaiot_dbg_api.h"
#include "remote_push_errno.h"
#include "remote_push_print.h"
#include "remote_push_io.h"
#include "inifile.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define SERVER_URL_MAX_LENGTH 256
#define INI_SECTION_SYS				"SYSTEM"
#define INI_KEY_PUSHSERVER			"PUSHSERVER"
#define INI_KEY_PUSHSERVPORT		"PUSHSERVPORT"
#define INI_KEY_UUID				"UUID"
#define INI_KEY_DEVID				"DEVID"
#define INI_KEY_DEVAKEY				"DEVAKEY" /* authorization key between device and server */
#define INI_KEY_TUTK_COMPATIBLE				"TUTKCOMPATIBLE"	

#define INI_CFG_FILENAME 			"/mnt/diskb/UDF/DOORBELL.CFG"

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/ 
typedef unsigned int UINT32;

/**************************************************************************
 *                           GLOBAL VARIABLES                             *
 **************************************************************************/
static int g_tiny_aiot_cnt = 0;
static tinyaiotOauth2Client_t gClient = {
	#if 0
	.id     = "icatch_smarthome",
	.secret = "123456",
	.redirect_uri  = "https://www.echo.icatchtek.com/oauth2/redirect",
	#else	/* */
	.id     = "bpsc_smarthome",
	.secret = "123456",
	.redirect_uri  = "https://www.echo.icatchtek.com/oauth2/redirect",
	#endif
};

static char *servUrl = NULL;
static char gUUID[21] = {0};
static char gDEVID[48] = {0};
static char gDevAkey[64] = {0};/*device password, needed by server*/
static int gTutkCompatible = 0;	/* 0 means not compatible, 1 means compatible */
/**************************************************************************
 *                           FUNCTION DECLARATION                           *
 **************************************************************************/



/**************************************************************************
 *                           FUNCTION DEFINITION                           *
 **************************************************************************/
int remotepushModuleTutkCompatibleModeGet(UINT32 syncFromFlash)
{
	if(1 == syncFromFlash) {
		/*tutk compatible mode*/
		char str_value[10] = {0};
		int iRet = 0;
		iRet = iniGet(INI_SECTION_SYS, INI_KEY_TUTK_COMPATIBLE, "0", str_value, sizeof(str_value), INI_CFG_FILENAME);
		if(!iRet) {
			return REMOTE_PUSH_ERRNO_OTHERS;
		}
		
		gTutkCompatible = atoi(str_value);
	}
	
	return gTutkCompatible;
}

static remotePushErrno_e remotePushModuleUUIDGet(char **ppUUID)
{
	if(0 == strlen(gUUID)) {
		/* read from flash */
		char str_value[128] = {0};
		
	   	int iRet = 0;
		iRet = iniGet(INI_SECTION_SYS, INI_KEY_UUID, "", str_value, sizeof(str_value), INI_CFG_FILENAME);
		if(!iRet) {
			return REMOTE_PUSH_ERRNO_OTHERS;
		}
		memcpy(gUUID, str_value, strlen(str_value));
	}
	
	*ppUUID = gUUID;
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

static remotePushErrno_e remotePushModuleDEVIDGet(char **ppDEVID)
{
	if(0 == strlen(gDEVID)) {
		/* read from flash */
		char str_value[128] = {0};
	   	int iRet = 0;
		iRet = iniGet(INI_SECTION_SYS, INI_KEY_DEVID,"", str_value, sizeof(str_value), INI_CFG_FILENAME);
		if(!iRet) {
			return REMOTE_PUSH_ERRNO_OTHERS;
		}
		memcpy(gDEVID, str_value, strlen(str_value));
	}
	
	*ppDEVID = gDEVID;
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

static remotePushErrno_e remotePushModuleDEVAKEYGet(char **ppDEVID)
{
	if(0 == strlen(gDevAkey)) {
		/* read from flash */
		char str_value[128] = {0};
	   	int iRet = 0;
		iRet = iniGet(INI_SECTION_SYS, INI_KEY_DEVAKEY,"", str_value, sizeof(str_value), INI_CFG_FILENAME);
		if(!iRet) {
			return REMOTE_PUSH_ERRNO_OTHERS;
		}
		memcpy(gDevAkey, str_value, strlen(str_value));
	}
	
	*ppDEVID = gDevAkey;
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

static int tinyaiotSaveDevInfoCB(const tinyaiotDevInfo_t* devInfo)
{
#if 0
	/* check password to determine save info or not */
	if(NULL == devInfo) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d]params error", __FUNCTION__,__LINE__);
		return -1;
	}
	if(devInfo->id) {
		if(FAIL == appSetDeviceID(devInfo->id)) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d] appSetDeviceID error", __FUNCTION__,__LINE__);
			return -1;
		}
	}
	if(devInfo->password && strlen(devInfo->password)) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d]", __FUNCTION__,__LINE__);
		/* save info */
		if(appSyncDevInfoToFlash((void*)devInfo)<0) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d]", __FUNCTION__,__LINE__);
			return -1;
		}
		if(FAIL == appSetSystemUUID(devInfo->uid)) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d]", __FUNCTION__,__LINE__);
			return -1;
		}
		if(FAIL == appSetDevPassword(devInfo->password)) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d]", __FUNCTION__,__LINE__);
			return -1;
		}
	}
	else {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d] The current app account isn't the owner of this device", __FUNCTION__,__LINE__);	
	}
#endif
	return 0;
}

static int tinyaiotReadDevInfoCB(tinyaiotSaveDevInfoArray_t* devInfo)
{
	/* uuid */
	char *pUUID = NULL;
	remotePushErrno_e eRet = remotePushModuleUUIDGet(&pUUID);
	if(REMOTE_PUSH_ERRNO_SUCCESS == eRet) {
		memcpy(devInfo->uid, pUUID, strlen(pUUID));
		dbgLogPrint(REMOTE_PUSH_LVL_INFO, "[%s-%d] uuid(%s)", __FUNCTION__,__LINE__, devInfo->uid);
	}
	
	
	
	/*device ID*/
	char *pdevID = NULL;
	eRet = remotePushModuleDEVIDGet(&pdevID);
	if(REMOTE_PUSH_ERRNO_SUCCESS != eRet) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d] get DEVID error", __FUNCTION__,__LINE__);
		if(NULL != pUUID) {
			memcpy(devInfo->id, pUUID, strlen(pUUID));
		}
	}
	else {
		memcpy(devInfo->id, pdevID, strlen(pdevID));
	}
	
	dbgLogPrint(REMOTE_PUSH_LVL_INFO, "[%s-%d] devID(%s)", __FUNCTION__,__LINE__, devInfo->id);
	/* dev password */
	char *pDEVAKEY = NULL;
	eRet = remotePushModuleDEVAKEYGet(&pDEVAKEY);
	if(REMOTE_PUSH_ERRNO_SUCCESS != eRet) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d] get DEVAKEY error", __FUNCTION__,__LINE__);
		return 0;
	}
	memcpy(devInfo->password, pDEVAKEY, strlen(pDEVAKEY));
	dbgLogPrint(REMOTE_PUSH_LVL_INFO, "[%s-%d] devpwd(%s)", __FUNCTION__,__LINE__, devInfo->password);
	return 0;
}
/**************************************************************
 *Name:remotePushModuleIOInit
 *Brief:Device initialize tinyAIoT to process setup flow
 *Note:This interface is implemented with tinyAIoT_Initialize2
 **************************************************************/
static remotePushErrno_e remotePushModuleIOInit(tinyaiotTransTypeId_e itransType, tinyaiotDevInfoUpdateCallbackFunction_t saveHandler, tinyaiotDevInfoGetCallbackFunction_t readHandler)
{
	int iRet = 0;
	g_tiny_aiot_cnt++;
	dbgLogPrint(REMOTE_PUSH_LVL_DEBUG, "[%s-%d]g_tiny_aiot_cnt(%d)", __FUNCTION__,__LINE__, g_tiny_aiot_cnt);
	if(1 == g_tiny_aiot_cnt) {
		iRet = tinyAIoT_Initialize2(itransType,saveHandler,readHandler);
		if(iRet) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d] ERRORNO(%d)", __FUNCTION__,__LINE__,iRet);
			g_tiny_aiot_cnt--;
			return REMOTE_PUSH_ERRNO_OTHERS;
		}
		iRet = tinyAIoT_SystemSet(TINYAIOT_PARAM_ID_OAUTH2_CLIENTS, (void*)&gClient);
		if (iRet) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d] ERRORNO(%d)", __FUNCTION__,__LINE__,iRet);
			g_tiny_aiot_cnt--;
			return REMOTE_PUSH_ERRNO_OTHERS;
		}
	}
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

/**************************************************************
 *Name:remotePushModuleIODeinit
 *Brief:Device deinitialize tinyAIoT to process setup flow
 **************************************************************/
remotePushErrno_e remotePushModuleIODeinit(void)
{
	int iRet = 0;
	g_tiny_aiot_cnt--;
	dbgLogPrint(REMOTE_PUSH_LVL_DEBUG, "[%s-%d] g_tiny_aiot_cnt(%d)", __FUNCTION__,__LINE__, g_tiny_aiot_cnt);
	if(0 == g_tiny_aiot_cnt) {
		iRet = tinyAIoT_DeInitialize();
		if(iRet) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d] ERRORNO(%d)", __FUNCTION__,__LINE__,iRet);
			g_tiny_aiot_cnt++;
		return REMOTE_PUSH_ERRNO_OTHERS;
		}
	}

	if(NULL == servUrl) {
		free(servUrl);
		servUrl = NULL;
	}
	return REMOTE_PUSH_ERRNO_SUCCESS;
}


static remotePushErrno_e remotePushModuleServUrlGet(char **ppServURL)
{
	if(NULL == servUrl) {
		servUrl = (char*)malloc(SERVER_URL_MAX_LENGTH);
		if(NULL == servUrl) {
			return REMOTE_PUSH_ERRNO_NOMEM;
		}
		memset(servUrl, 0, SERVER_URL_MAX_LENGTH);
	}	

	if(0 == strlen(servUrl)) {
		/* read from flash */
		char str_value[128] = {0};
		
	   	int iRet = 0;
		/* port */
		iRet = iniGet(INI_SECTION_SYS, INI_KEY_PUSHSERVPORT, "", str_value, sizeof(str_value), INI_CFG_FILENAME);
		if(!iRet) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d] get push server port error", __FUNCTION__,__LINE__);
			return REMOTE_PUSH_ERRNO_OTHERS;
		}
		int port = atoi(str_value);
			
		/* address */
		memset(str_value, 0, sizeof(str_value));
		iRet = iniGet(INI_SECTION_SYS, INI_KEY_PUSHSERVER, "", str_value, sizeof(str_value), INI_CFG_FILENAME);
		if(!iRet) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d] get push server address error", __FUNCTION__,__LINE__);
			return REMOTE_PUSH_ERRNO_OTHERS;
		}
		snprintf(servUrl, SERVER_URL_MAX_LENGTH, "%s:%d", str_value, port);
	}
	
	*ppServURL = servUrl;
	
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

remotePushErrno_e remotePushModuleSendMsg(remotePushGenericMsgParam_t *genericParam)
{
	remotePushErrno_e eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	tinyaiotMessage_t *msgInfo = NULL;
	if (NULL == genericParam) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:param illegal!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_PARAM;
		goto Exit;
	}
	//tinyAIoT_DbgPrioritySet(TINYAIOT_LOG_DEBUG);
	dbgLogPrint(REMOTE_PUSH_LVL_WARN,"[%s:%d]version(0x%08x)!",__FUNCTION__,__LINE__, tinyAIoT_GetApiVer());
	if(REMOTE_PUSH_ERRNO_SUCCESS != remotePushModuleIOInit(TINYAIOT_TRANS_TYPE_ID_HTTP, tinyaiotSaveDevInfoCB, tinyaiotReadDevInfoCB)) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:remotePushModuleIOInit fail!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_OTHERS;
		goto Exit;
	}
	
	char *pServURL = NULL;
	eRet = remotePushModuleServUrlGet(&pServURL);
	if(REMOTE_PUSH_ERRNO_SUCCESS != eRet) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error get server URL!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_OTHERS;
		goto Exit;
	}

	dbgLogPrint(REMOTE_PUSH_LVL_INFO, "[%s-%d] ServURL[%s]", __FUNCTION__,__LINE__,pServURL);
	int iRet = tinyAIoT_SystemSet(TINYAIOT_PARAM_ID_DEV_SERVER_ADDR, pServURL);
	if(0 != iRet) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d] set server URL fail", __FUNCTION__,__LINE__);
		return REMOTE_PUSH_ERRNO_OTHERS;	
	}
	
	msgInfo = (tinyaiotMessage_t*)malloc(sizeof(tinyaiotMessage_t));
	if (NULL == msgInfo) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:malloc memory fail!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_NOMEM;
		goto Exit;
	}
	
	memset(msgInfo, 0, sizeof(tinyaiotMessage_t));
	msgInfo->msgId = genericParam->msgId;
	msgInfo->msgType = genericParam->msgType;
	msgInfo->time = genericParam->time;
	msgInfo->msgParam = genericParam->msgParam;
	msgInfo->extraParam = genericParam->extraParam;
	msgInfo->attachment = genericParam->attachment;
	msgInfo->errCode = genericParam->errCode;
	msgInfo->format = genericParam->format;
	dbgLogPrint(REMOTE_PUSH_LVL_INFO,"[%s:%d]msgStr(%s)!",__FUNCTION__,__LINE__, msgInfo->extraParam);
	
	UINT32 retry = 2;
	while(retry--) {
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"@@push msg to server start");
		iRet = tinyAIoT_MsgNotify((void*)msgInfo);
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"push msg to server end");
		if(0 == iRet) {
			break;
		}
		usleep(100*1000);
	}
	if(iRet) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:TinyAIot_MsgNotify fail, errcode(%d)",__FUNCTION__,__LINE__, iRet);
		eRet = REMOTE_PUSH_ERRNO_SEND;
		goto Exit;
	}
	eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	dbgLogPrint(REMOTE_PUSH_LVL_INFO,"[%s:%d]TinyAIot_MsgNotify succeed.!",__FUNCTION__,__LINE__);
	
	Exit:
		if(genericParam) {
			free(genericParam->extraParam);
			free(genericParam->msgParam);
			free(genericParam->attachment);
		}
		if(msgInfo) {
			free(msgInfo);
		}
	return eRet;
}

remotePushErrno_e remotePushModuleSendFile(remotePushGenericFileParam_t *genericParam)
{
	int eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	if (NULL == genericParam) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:param illegal!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_PARAM;
		goto Exit;
	}
	//tinyAIoT_DbgPrioritySet(TINYAIOT_LOG_DEBUG);
	dbgLogPrint(REMOTE_PUSH_LVL_WARN,"[%s:%d]version(0x%08x)!",__FUNCTION__,__LINE__, tinyAIoT_GetApiVer());
	if(REMOTE_PUSH_ERRNO_SUCCESS != remotePushModuleIOInit(TINYAIOT_TRANS_TYPE_ID_HTTP, tinyaiotSaveDevInfoCB, tinyaiotReadDevInfoCB)) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:remotePushModuleIOInit fail!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_OTHERS;
		goto Exit;
	}
	
	char *pServURL = NULL;
	eRet = remotePushModuleServUrlGet(&pServURL);
	if(REMOTE_PUSH_ERRNO_SUCCESS != eRet) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error get server URL!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_OTHERS;
		goto Exit;
	}

	dbgLogPrint(REMOTE_PUSH_LVL_INFO, "[%s-%d] ServURL[%s]", __FUNCTION__,__LINE__,pServURL);
	int iRet = tinyAIoT_SystemSet(TINYAIOT_PARAM_ID_DEV_SERVER_ADDR, pServURL);
	if(0 != iRet) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s-%d] set server address fail", __FUNCTION__,__LINE__);
		return REMOTE_PUSH_ERRNO_OTHERS;	
	}

	tinyaiotFileInfo_t fileInfo = {0};
	fileInfo.chunked = 0;
	fileInfo.iso_time = genericParam->timeStr;
	fileInfo.data = (char*)genericParam->data;
	fileInfo.size = genericParam->size;
	
	UINT32 retry = 2;

	while(retry--) {
		iRet = tinyAIoT_Upload(genericParam->destName, genericParam->dataType, &fileInfo);
		if(0 == iRet) {
			break;
		}
		usleep(50*1000);
	}
	if(iRet) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:Tinyaiot_upload fail,errcode(%d)",__FUNCTION__,__LINE__, iRet);
		eRet = REMOTE_PUSH_ERRNO_SEND;
	}
	else {
		dbgLogPrint(REMOTE_PUSH_LVL_INFO,"[%s:%d]Tinyaiot_upload succeed!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	}
	Exit:
		if(genericParam->data) {
			free(genericParam->data);
		}
		return eRet;
}

