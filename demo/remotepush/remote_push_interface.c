/*
 *File Name:remote_push_interface.c
 *Brief:Push message or file to remote mobile App
 *Date:2020-08-04
 */

#define REMOTE_PUSH_PRINT_EN  1
#define REMOTE_PUSH_DBG_DEF_LEVEL		REMOTE_PUSH_LVL_DEBUG

/**************************************************************************
 *                           I N C L U D E S	                           *
 **************************************************************************/
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "remote_push_queue.h"
#include "remote_push_errno.h"
#include "remote_push_print.h"
#include "remote_push_interface.h"
#include "remote_push_io.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define WAIT_FOREVER	(0xFFFFFFFF)

#define TRUE	1
#define FALSE	0

#define RING_MSG_FORMAT "{\"result\":%d,\"faceId\":[%s],\"timeInSecs\":%d,\"locationInfos\":[%s]}"
#define PIR_MSG_FORMAT "{\"timeInSecs\":%d,\"locationInfos\":[%s]}"
#define PACKAGE_DETECT_MSG_FORMAT "{\"timeInSecs\":%d,\"locationInfos\":[%s]}"
#define LOCATIONINFO_FORMAT	"{\"left\":%d, \"top\":%d, \"width\":%d, \"height\":%d}"
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/ 
typedef unsigned char BOOL;

/**************************************************************************
 *                           GLOBAL VARIABLES                             *
 **************************************************************************/
static pthread_t gMsgTaskID = 0;
static pthread_t gFileTaskID = 0;
static remotePushCBList_t gRemotePushCBList = {0};
static remotePushQueue *pMsgQueue = NULL;
static remotePushQueue *pFileQueue = NULL;
static BOOL gLoopMsg = TRUE;
static BOOL gLoopFile = TRUE;
static UINT32 gMsgCount = 0;
/**************************************************************************
 *                           FUNCTION DECLARATION                           *
 **************************************************************************/



/**************************************************************************
 *                           FUNCTION DEFINITION                           *
 **************************************************************************/
 
/*
 *Name: remotePushModuleCBRegister
 *Brief:Register necessary callback function
 *Args:sel---callback selection, cb---callback pointer
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
remotePushErrno_e remotePushModuleCBRegister(remotePushCBs_e sel, remotePushCBType cb)
{
	switch(sel) {
		case REMOTE_PUSH_CB_MSG_START_COUNTDOWN:
			gRemotePushCBList.msgStartPwrOffCB =  cb;
			break;
		case REMOTE_PUSH_CB_MSG_STOP_COUNTDOWN:
			gRemotePushCBList.msgStopPwrOffCB =  cb;
			break;
		case REMOTE_PUSH_CB_FILE_START_COUNTDOWN:
			gRemotePushCBList.fileStartPwrOffCB =  cb;
			break;
		case REMOTE_PUSH_CB_FILE_STOP_COUNTDOWN:
			gRemotePushCBList.fileStopPwrOffCB =  cb;
			break;
		default:	
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:param illegal!",__FUNCTION__,__LINE__);
		return REMOTE_PUSH_ERRNO_PARAM;
	}
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

/*
 *Name: remotePushModuleMsgPushTrigger
 *Brief:[enqueue]push a message to the remote mobile App
 *Args:remotePushMsgBody_t *msgBody ---parameters of message
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
remotePushErrno_e remotePushModuleMsgPushTrigger(remotePushMsgBody_t *msgBody)
{
	if(NULL == pMsgQueue) {
		remotePushErrno_e eRet = remotePushQueueCreate(&pMsgQueue);
		if(REMOTE_PUSH_ERRNO_SUCCESS != eRet) {
			return eRet;
		}
	}

	/* Transfer parameters to another memory */
	remotePushMsgBody_t *pMsgBody = (remotePushMsgBody_t*)malloc(sizeof(remotePushMsgBody_t));
	if(NULL == pMsgBody) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:malloc buffer failed!",__FUNCTION__,__LINE__);
		return REMOTE_PUSH_ERRNO_NOMEM;
	}
	memcpy(pMsgBody, msgBody, sizeof(remotePushMsgBody_t));
	if(msgBody->baseParam.data && msgBody->baseParam.size) {
		char* data = (char*)malloc(msgBody->baseParam.size);
		if(NULL == data) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:malloc buffer failed!",__FUNCTION__,__LINE__);
			free(pMsgBody);
			return REMOTE_PUSH_ERRNO_NOMEM;
		}
		memcpy(data, msgBody->baseParam.data, msgBody->baseParam.size);
		pMsgBody->baseParam.data = data;
		pMsgBody->baseParam.size = msgBody->baseParam.size;
	}
	
	/* Enqueue */
	remotePushQueueNode_t *pNode = (remotePushQueueNode_t*)malloc(sizeof(remotePushQueueNode_t));
	pNode->data = (void*)pMsgBody;
	pNode->dataSize = sizeof(remotePushMsgBody_t);
	
	if(REMOTE_PUSH_ERRNO_SUCCESS != remotePushQueueSend(pMsgQueue, pNode)) {/* enqueue fail */
		free(pNode->data);
		free(pNode);
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:enqueue fail!",__FUNCTION__,__LINE__);
		return REMOTE_PUSH_ERRNO_SEND;
	}
	
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

/*
 *Name: remotePushModuleFilePushTrigger
 *Brief:[enqueue]push file or big data to the remote mobile App
 *Args:remotePushFileBody_t *fileBody --- paramters of file information
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
remotePushErrno_e remotePushModuleFilePushTrigger(remotePushFileBody_t *fileBody)
{
	if(NULL == pFileQueue) {
		remotePushErrno_e eRet = remotePushQueueCreate(&pFileQueue);
		if(REMOTE_PUSH_ERRNO_SUCCESS != eRet) {
			return eRet;
		}
	}
	
	/* Transfer parameters to another memory */
	remotePushFileBody_t *pFileBody = (remotePushFileBody_t*)malloc(sizeof(remotePushFileBody_t));
	if(NULL == pFileBody) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:malloc buffer failed!",__FUNCTION__,__LINE__);
		return REMOTE_PUSH_ERRNO_NOMEM;
	}
	memcpy(pFileBody, fileBody, sizeof(remotePushFileBody_t));
	if(fileBody->data && fileBody->size) {
		char* data = (char*)malloc(fileBody->size);
		if(NULL == data) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:malloc buffer failed!",__FUNCTION__,__LINE__);
			free(pFileBody);
			return REMOTE_PUSH_ERRNO_NOMEM;
		}
		memcpy(data, fileBody->data, fileBody->size);
		pFileBody->data = data;
		pFileBody->size = fileBody->size;
	}
	/* Enqueue */
	remotePushQueueNode_t *pNode = (remotePushQueueNode_t*)malloc(sizeof(remotePushQueueNode_t));
	pNode->data = (void*)pFileBody;
	pNode->dataSize = sizeof(remotePushFileBody_t);
	if(REMOTE_PUSH_ERRNO_SUCCESS != remotePushQueueSend(pFileQueue, pNode)) {/* enqueue fail */
		free(pNode->data);
		free(pNode);
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:enqueue fail!",__FUNCTION__,__LINE__);
		return REMOTE_PUSH_ERRNO_SEND;
	}
	
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

/*
 *Name: remotePushModuleWifiConnectedQuery
 *Brief:Judge whether Wifi is connected  or not.
 *Args:None
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushModuleWifiConnectedQuery()
{
/*
	if(WIFI_STA_MODE != appWiFiStationGetStatus()) {
		return REMOTE_PUSH_ERRNO_OTHERS;
	}
*/
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

/*
 *Name: remotePushModuleRingEvtWrapper
 *Brief:convert parameters for ring message.
 *Args:remotePushMsgBody_t *msgBody --- pointer to message;remotePushGenericMsgParam_t *genericParam --- converted parameters
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushModuleRingEvtWrapper(remotePushMsgBody_t *msgBody, remotePushGenericMsgParam_t *genericParam)
{
	remotePushErrno_e eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	char *msgStr = NULL;
	if((NULL == msgBody) || (NULL == genericParam)) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:param illegal!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_PARAM;
		goto Exit;
	}
	
	msgStr = (char*)malloc(512);
	if(NULL == msgStr) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:malloc buffer failed!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_NOMEM;
		goto Exit;
	}
	dbgLogPrint(REMOTE_PUSH_LVL_DEBUG,"[%s:%d]coordinationGrpNum(%d)!",__FUNCTION__,__LINE__, msgBody->msgCenterParam.coordinationGrpNum);
	/* wrapping face ID list */
	char *faceIDs = NULL;
	if(0 != msgBody->faceRecogParam.faceIDNum) {
		faceIDs = (char*)malloc(msgBody->faceRecogParam.faceIDNum*16);
		if(NULL == faceIDs) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:malloc buffer failed!",__FUNCTION__,__LINE__);
			return REMOTE_PUSH_ERRNO_NOMEM;
		}
		memset(faceIDs, 0, msgBody->faceRecogParam.faceIDNum*16);
		int idx = 0;
		for(idx=0; idx<msgBody->faceRecogParam.faceIDNum; idx++) {
			if((msgBody->faceRecogParam.faceIDNum>=1) && (idx == msgBody->faceRecogParam.faceIDNum-1)) {
				sprintf(faceIDs+strlen(faceIDs), "%d", msgBody->faceRecogParam.faceID[idx]);
			}
			else {
				sprintf(faceIDs+strlen(faceIDs), "%d,", msgBody->faceRecogParam.faceID[idx]);
			}
		}
	}
	dbgLogPrint(REMOTE_PUSH_LVL_DEBUG,"[%s:%d]faceIDs(%s)!",__FUNCTION__,__LINE__, faceIDs);
	/* wrapping face location coordinate */
	if(0 != msgBody->msgCenterParam.coordinationGrpNum) {
		char *locations = malloc(msgBody->msgCenterParam.coordinationGrpNum*64);
		if(NULL == locations) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:malloc buffer failed!",__FUNCTION__,__LINE__);
			return REMOTE_PUSH_ERRNO_NOMEM;
		}
		memset(locations, 0, sizeof(msgBody->msgCenterParam.coordinationGrpNum*64));
		int idx = 0;
		for(idx=0; idx < msgBody->msgCenterParam.coordinationGrpNum; idx++) {
			sprintf(locations+strlen(locations), LOCATIONINFO_FORMAT, msgBody->msgCenterParam.coordinate[idx].left, msgBody->msgCenterParam.coordinate[idx].top, msgBody->msgCenterParam.coordinate[idx].width, msgBody->msgCenterParam.coordinate[idx].height);
			if(msgBody->msgCenterParam.coordinationGrpNum>1 && idx < (msgBody->msgCenterParam.coordinationGrpNum-1)) {
				strcat(locations, ",");
			}	
		}
		sprintf(msgStr, RING_MSG_FORMAT, msgBody->faceRecogParam.recogResult, faceIDs, msgBody->msgCenterParam.timeStamp, locations);
		free(locations);
	}
	else {
		sprintf(msgStr, RING_MSG_FORMAT, msgBody->faceRecogParam.recogResult, "", msgBody->msgCenterParam.timeStamp,"");
	}
	dbgLogPrint(REMOTE_PUSH_LVL_DEBUG,"[%s:%d]msgStr(%s)!",__FUNCTION__,__LINE__, msgStr);
	genericParam->msgId = gMsgCount++;
	genericParam->msgType = msgBody->baseParam.type;	
	memcpy(genericParam->time, msgBody->baseParam.timeStr, strlen(msgBody->baseParam.timeStr));
	genericParam->extraParam = msgStr;
	genericParam->msgParam = (char*)msgBody->baseParam.data;

	if(1 == remotepushModuleTutkCompatibleModeGet(0)) {
		genericParam->format = 4; /* support Tutk format */
	}
	
	return REMOTE_PUSH_ERRNO_SUCCESS;	
	
	Exit:
		if(msgStr) {
			free(msgStr);
		}
	return eRet;
}

/*
 *Name: remotePushModuleMotionDetectEvtWrapper
 *Brief:convert parameters for motion detection message.
 *Args:remotePushMsgBody_t *msgBody --- pointer to message;remotePushGenericMsgParam_t *genericParam --- converted parameters
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushModuleMotionDetectEvtWrapper(remotePushMsgBody_t *msgBody, remotePushGenericMsgParam_t *genericParam)
{
	remotePushErrno_e eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	char *msgStr = NULL;
	if((NULL == msgBody) || (NULL == genericParam)) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:param illegal!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_PARAM;
		goto Exit;
	}
	
	msgStr = (char*)malloc(512);
	if(NULL == msgStr) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:malloc buffer failed!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_NOMEM;
		goto Exit;
	}
	dbgLogPrint(REMOTE_PUSH_LVL_DEBUG,"[%s:%d]coordinationGrpNum(%d)!",__FUNCTION__,__LINE__, msgBody->msgCenterParam.coordinationGrpNum);
	/* wrapping human-shape coordinate */
	if(0 != msgBody->msgCenterParam.coordinationGrpNum) {
		char *locations = malloc(msgBody->msgCenterParam.coordinationGrpNum*64);
		if(NULL == locations) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:malloc buffer failed!",__FUNCTION__,__LINE__);
			return REMOTE_PUSH_ERRNO_NOMEM;
		}
		memset(locations, 0, sizeof(msgBody->msgCenterParam.coordinationGrpNum*64));
		int idx = 0;
		for(idx=0; idx < msgBody->msgCenterParam.coordinationGrpNum; idx++) {
			sprintf(locations+strlen(locations), LOCATIONINFO_FORMAT, msgBody->msgCenterParam.coordinate[idx].left, msgBody->msgCenterParam.coordinate[idx].top, msgBody->msgCenterParam.coordinate[idx].width, msgBody->msgCenterParam.coordinate[idx].height);
			if(msgBody->msgCenterParam.coordinationGrpNum>1 && idx < (msgBody->msgCenterParam.coordinationGrpNum-1)) {
				strcat(locations, ",");
			}	
		}
		sprintf(msgStr, PIR_MSG_FORMAT, msgBody->msgCenterParam.timeStamp, locations);
		free(locations);
	}
	else {
		sprintf(msgStr, PIR_MSG_FORMAT, msgBody->msgCenterParam.timeStamp,"");
	}
	
	genericParam->msgId = gMsgCount++;
	genericParam->msgType = msgBody->baseParam.type;	
	memcpy(genericParam->time, msgBody->baseParam.timeStr, strlen(msgBody->baseParam.timeStr));
	genericParam->extraParam = msgStr;
	genericParam->msgParam = (char*)msgBody->baseParam.data;
	if(1 == remotepushModuleTutkCompatibleModeGet(0)) {
		genericParam->format = 4; /* support Tutk format */
	}
	
	return REMOTE_PUSH_ERRNO_SUCCESS;	
	
	Exit:
		if(msgStr) {
			free(msgStr);
		}
	return eRet;
}

/*
 *Name: remotePushModuleLowPowerEvtWrapper
 *Brief:convert parameters for low-power message.
 *Args:remotePushMsgBody_t *msgBody --- pointer to message;remotePushGenericMsgParam_t *genericParam --- converted parameters
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushModuleLowPowerEvtWrapper(remotePushMsgBody_t *msgBody, remotePushGenericMsgParam_t *genericParam)
{
	remotePushErrno_e eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	char *msgStr = NULL;
	if((NULL == msgBody) || (NULL == genericParam)) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:param illegal!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_PARAM;
		goto Exit;
	}
	
	genericParam->msgId = gMsgCount++;
	genericParam->msgType = msgBody->baseParam.type;	
	memcpy(genericParam->time, msgBody->baseParam.timeStr, strlen(msgBody->baseParam.timeStr));
	genericParam->extraParam = msgStr;
	genericParam->msgParam = (char*)msgBody->baseParam.data;
	if(1 == remotepushModuleTutkCompatibleModeGet(0)) {
		genericParam->format = 4; /* support Tutk format */
	}
	
	return REMOTE_PUSH_ERRNO_SUCCESS;	
	
	Exit:
		if(msgStr) {
			free(msgStr);
		}
	return eRet;
}
/*
 *Name: remotePushModulePackageDetectEvtWrapper
 *Brief:convert parameters for package detection message.
 *Args:remotePushMsgBody_t *msgBody --- pointer to message;remotePushGenericMsgParam_t *genericParam --- converted parameters
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushModulePackageDetectEvtWrapper(remotePushMsgBody_t *msgBody, remotePushGenericMsgParam_t *genericParam)
{
	remotePushErrno_e eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	char *msgStr = NULL;
	if((NULL == msgBody) || (NULL == genericParam)) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:param illegal!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_PARAM;
		goto Exit;
	}
	
	msgStr = (char*)malloc(512);
	if(NULL == msgStr) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:malloc buffer failed!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_NOMEM;
		goto Exit;
	}
	dbgLogPrint(REMOTE_PUSH_LVL_DEBUG,"[%s:%d]coordinationGrpNum(%d)!",__FUNCTION__,__LINE__, msgBody->msgCenterParam.coordinationGrpNum);
	/* wrapping package location coordinate */
	if(0 != msgBody->msgCenterParam.coordinationGrpNum) {
		char *locations = malloc(msgBody->msgCenterParam.coordinationGrpNum*64);
		if(NULL == locations) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:malloc buffer failed!",__FUNCTION__,__LINE__);
			return REMOTE_PUSH_ERRNO_NOMEM;
		}
		memset(locations, 0, sizeof(msgBody->msgCenterParam.coordinationGrpNum*64));
		int idx = 0;
		for(idx=0; idx < msgBody->msgCenterParam.coordinationGrpNum; idx++) {
			sprintf(locations+strlen(locations), LOCATIONINFO_FORMAT, msgBody->msgCenterParam.coordinate[idx].left, msgBody->msgCenterParam.coordinate[idx].top, msgBody->msgCenterParam.coordinate[idx].width, msgBody->msgCenterParam.coordinate[idx].height);
			if(msgBody->msgCenterParam.coordinationGrpNum>1 && idx < (msgBody->msgCenterParam.coordinationGrpNum-1)) {
				strcat(locations, ",");
			}	
		}
		sprintf(msgStr, PACKAGE_DETECT_MSG_FORMAT, msgBody->msgCenterParam.timeStamp, locations);
		free(locations);
	}
	else {
		sprintf(msgStr, PACKAGE_DETECT_MSG_FORMAT, msgBody->msgCenterParam.timeStamp,"");
	}
	
	genericParam->msgId = gMsgCount++;
	genericParam->msgType = msgBody->baseParam.type;	
	memcpy(genericParam->time, msgBody->baseParam.timeStr, strlen(msgBody->baseParam.timeStr));
	genericParam->extraParam = msgStr;
	genericParam->msgParam = (char*)msgBody->baseParam.data;
	if(1 == remotepushModuleTutkCompatibleModeGet(0)) {
		genericParam->format = 4; /* support Tutk format */
	}
	
	return REMOTE_PUSH_ERRNO_SUCCESS;	
	
	Exit:
		if(msgStr) {
			free(msgStr);
		}
	return eRet;
}

/*
 *Name: remotePushModuleExitPVEvtWrapper
 *Brief:convert parameters for exit-preview message.
 *Args:remotePushMsgBody_t *msgBody --- pointer to message;remotePushGenericMsgParam_t *genericParam --- converted parameters
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushModuleExitPVEvtWrapper(remotePushMsgBody_t *msgBody, remotePushGenericMsgParam_t *genericParam)
{
	remotePushErrno_e eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	char *msgStr = NULL;
	if((NULL == msgBody) || (NULL == genericParam)) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:param illegal!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_PARAM;
		goto Exit;
	}
	
	genericParam->msgId = gMsgCount++;
	genericParam->msgType = msgBody->baseParam.type;	
	memcpy(genericParam->time, msgBody->baseParam.timeStr, strlen(msgBody->baseParam.timeStr));
	genericParam->extraParam = msgStr;
	genericParam->msgParam = (char*)msgBody->baseParam.data;
	if(1 == remotepushModuleTutkCompatibleModeGet(0)) {
		genericParam->format = 4; /* support Tutk format */
	}
	
	return REMOTE_PUSH_ERRNO_SUCCESS;	
	
	Exit:
		if(msgStr) {
			free(msgStr);
		}
	return eRet;
}

/*
 *Name: remotePushModuleOTAEvtWrapper
 *Brief:convert parameters for OTA related message.
 *Args:remotePushMsgBody_t *msgBody --- pointer to message;remotePushGenericMsgParam_t *genericParam --- converted parameters
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushModuleOTAEvtWrapper(remotePushMsgBody_t *msgBody, remotePushGenericMsgParam_t *genericParam)
{
	remotePushErrno_e eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	char *msgStr = NULL;
	if((NULL == msgBody) || (NULL == genericParam)) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:param illegal!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_PARAM;
		goto Exit;
	}
	
	genericParam->msgId = gMsgCount++;
	genericParam->msgType = msgBody->baseParam.type;	
	memcpy(genericParam->time, msgBody->baseParam.timeStr, strlen(msgBody->baseParam.timeStr));
	genericParam->extraParam = msgStr;
	genericParam->msgParam = (char*)msgBody->baseParam.data;
	if(1 == remotepushModuleTutkCompatibleModeGet(0)) {
		genericParam->format = 4; /* support Tutk format */
	}
	
	return REMOTE_PUSH_ERRNO_SUCCESS;	
	
	Exit:
		if(msgStr) {
			free(msgStr);
		}
	return eRet;
}

/*
 *Name: remotePushModuleGeneralEvtWrapper
 *Brief:convert parameters for general message.
 *Args:remotePushMsgBody_t *msgBody --- pointer to message;remotePushGenericMsgParam_t *genericParam --- converted parameters
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushModuleGeneralEvtWrapper(remotePushMsgBody_t *msgBody, remotePushGenericMsgParam_t *genericParam)
{
	remotePushErrno_e eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	char *msgStr = NULL;
	if((NULL == msgBody) || (NULL == genericParam)) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:param illegal!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_PARAM;
		goto Exit;
	}
	
	genericParam->msgId = gMsgCount++;
	genericParam->msgType = msgBody->baseParam.type;	
	memcpy(genericParam->time, msgBody->baseParam.timeStr, strlen(msgBody->baseParam.timeStr));
	genericParam->extraParam = msgStr;
	genericParam->msgParam = (char*)msgBody->baseParam.data;
	if(1 == remotepushModuleTutkCompatibleModeGet(0)) {
		genericParam->format = 4; /* support Tutk format */
	}
	
	return REMOTE_PUSH_ERRNO_SUCCESS;	
	
	Exit:
		if(msgStr) {
			free(msgStr);
		}
	return eRet;
}

/*
 *Name: remotePushModuleFilePushWrapper
 *Brief:convert parameters for file-pusing
 *Args:remotePushFileBody_t *fileBody --- pointer to file information;remotePushGenericFileParam_t *genericParam = converted parameters
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushModuleFilePushWrapper(remotePushFileBody_t *fileBody, remotePushGenericFileParam_t *genericParam)
{
	if(NULL == fileBody || NULL == genericParam) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:param illegal!",__FUNCTION__,__LINE__);
		return REMOTE_PUSH_ERRNO_PARAM;
	}
	genericParam->data = fileBody->data;
	genericParam->size = fileBody->size;
	genericParam->dataType = fileBody->dataType;
	strcpy(genericParam->destName, fileBody->destName);
	strcpy(genericParam->timeStr, fileBody->timeStr);
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

/*
 *Name: remotePushModulePushFile
 *Brief:pushing file or big data.
 *Args:remotePushGenericMsgParam_t *genericParam --- generic type of message parameter 
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or FAIL)
 */
static remotePushErrno_e remotePushModulePushFile(remotePushGenericFileParam_t *genericParam)
{
	return remotePushModuleSendFile(genericParam);
}

/*
 *Name: remotePushModulePushMsg
 *Brief:pushing message
 *Args:remotePushGenericMsgParam_t *genericParam --- generic type of message parameter 
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushModulePushMsg(remotePushGenericMsgParam_t *genericParam)
{
	return remotePushModuleSendMsg(genericParam);
}

/*
 *Name: remotePushMsgProcess
 *Brief:Processing message, including wrapping message and pushing it.
 *Args:remotePushMsgBody_t *msgBody --- pointer to message
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushMsgProcess(remotePushMsgBody_t *msgBody)
{
	if(NULL == msgBody) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s:%d]error:parameter is NULL",__FUNCTION__ ,__LINE__);
		return REMOTE_PUSH_ERRNO_PARAM;
	}

	/* wrapping message */
	remotePushGenericMsgParam_t msgParam = {0};
	switch(msgBody->baseParam.type) {
		case MSG_EVT_RING:
			remotePushModuleRingEvtWrapper(msgBody, &msgParam);
			break;
		case MSG_EVT_MOTION_DET:
			remotePushModuleMotionDetectEvtWrapper(msgBody, &msgParam);
			break;
		case MSG_EVT_LOW_BAT:
			remotePushModuleLowPowerEvtWrapper(msgBody, &msgParam);
			break;
		case MSG_EVT_USER_PACKAGE_DETECT:
			remotePushModulePackageDetectEvtWrapper(msgBody, &msgParam);
			break;
		case MSG_EVT_EXITPV:
			remotePushModuleExitPVEvtWrapper(msgBody, &msgParam);
			break;
		case MSG_EVT_OTA_DOWNLOAD_DONE:
		case MSG_EVT_OTA_DOWNLOAD_ERR:
		case MSG_EVT_OTA_INSTALL_DONE:
		case MSG_EVT_OTA_INSTALL_ERR:
			remotePushModuleOTAEvtWrapper(msgBody, &msgParam);
			break;
		default:
			remotePushModuleGeneralEvtWrapper(msgBody, &msgParam);
			break;
	}
	
	/* pushing message */
	remotePushErrno_e eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	eRet = remotePushModulePushMsg(&msgParam);
	if(NULL != msgBody->resultCb) {
		msgBody->resultCb(msgBody->baseParam.type, eRet);
	}
	return eRet;
}


/*
 *Name: remotePushFileProcess
 *Brief:Deal with file or big data, including wrapping and pushing it.
 *Args:remotePushFileBody_t *fileBody --- pointer to file data
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushFileProcess(remotePushFileBody_t *fileBody)
{
	if(NULL == fileBody) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:param illegal!",__FUNCTION__,__LINE__);
		return REMOTE_PUSH_ERRNO_PARAM;
	}
	remotePushGenericFileParam_t genericParam;
	memset(&genericParam, 0, sizeof(remotePushGenericFileParam_t));
	remotePushModuleFilePushWrapper(fileBody, &genericParam);

	remotePushErrno_e eRet = REMOTE_PUSH_ERRNO_SUCCESS;
	eRet = remotePushModulePushFile(&genericParam);
	if(NULL != fileBody->resultCb) {
		fileBody->resultCb(eRet);
	}
	return eRet;
}

/*
 *Name: remotePushModuleMsgTask
 *Brief:Task to push ordinary message to remote moblie App
 *Args:void* arg
 *Return:void*
 */
void * remotePushModuleMsgTask(void *arg)
{
	if(NULL == pMsgQueue) {
		remotePushErrno_e eRet = remotePushQueueCreate(&pMsgQueue);
		if(REMOTE_PUSH_ERRNO_SUCCESS != eRet) {
			return NULL;
		}
	}
	
	remotePushMsgBody_t msgBody;
	while(gLoopMsg) {
		if(REMOTE_PUSH_ERRNO_SUCCESS != remotePushModuleWifiConnectedQuery()) {
			dbgLogPrint(REMOTE_PUSH_LVL_INFO, "[%s-%d]", __FUNCTION__,__LINE__);
			usleep(50*1000);
			continue;
		}
	
		remotePushQueueNode_t *pNode = NULL;
		remotePushQueueReceive(pMsgQueue, &pNode, WAIT_FOREVER);
		
		if(pNode) {
			
			/* copy parameters to another memory */
			memcpy(&msgBody, (void*)pNode->data, pNode->dataSize);
			free(pNode->data);
			free((void*)pNode); /* free the original memory immediately  */
			
			if(gRemotePushCBList.msgStopPwrOffCB) {/* invoke callback function to stop power-off countdown flow */
				gRemotePushCBList.msgStopPwrOffCB(NULL);
			}
			
			remotePushMsgProcess(&msgBody);
			memset(&msgBody, 0, sizeof(remotePushMsgBody_t));
			
			if(gRemotePushCBList.msgStartPwrOffCB) {/* invoke callback function to resume power-off countdown flow */
				gRemotePushCBList.msgStartPwrOffCB(NULL);
			}
		}
		usleep(50*1000);
	}
	remotePushQueueDestroy(&pMsgQueue);
	return NULL;
}

/*
 *Name: remotePushModuleFileTaskCreate
 *Brief:Task to push file or big data to remote moblie App
 *Args:void* arg
 *Return:void*
 */
void * remotePushModuleFileTask(void *arg)
{
	if(NULL == pFileQueue) {
		remotePushErrno_e eRet = remotePushQueueCreate(&pFileQueue);
		if(REMOTE_PUSH_ERRNO_SUCCESS != eRet) {
			return NULL;
		}
	}

	remotePushFileBody_t pFileBody;
	while(gLoopFile) {
		if(REMOTE_PUSH_ERRNO_SUCCESS != remotePushModuleWifiConnectedQuery()) {
			usleep(50*1000);
			continue;
		}
		remotePushQueueNode_t *pNode = NULL;
		remotePushQueueReceive(pFileQueue, &pNode, WAIT_FOREVER);
		if(pNode) {
			memcpy(&pFileBody, (void*)pNode->data, pNode->dataSize);
			if(gRemotePushCBList.fileStopPwrOffCB) {/* invoke callback function to stop power-off countdown flow */
				gRemotePushCBList.fileStopPwrOffCB(NULL);
			}
			remotePushFileProcess(&pFileBody);
			memset(&pFileBody, 0, sizeof(remotePushFileBody_t));
			if(gRemotePushCBList.fileStartPwrOffCB) {/* invoke callback function to resume power-off countdown flow */
				gRemotePushCBList.fileStartPwrOffCB(NULL);
			}	
		}
		usleep(50*1000);	
	}
	remotePushQueueDestroy(&pFileQueue);
	return NULL;
}

/*
 *Name: remotePushModuleMsgTaskCreate
 *Brief:create a task to push ordinary message to remote moblie App
 *Args:None
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushModuleMsgTaskCreate()
{
	if(0 == gMsgTaskID) {
		/*pthread setting*/
		pthread_attr_t attr;
		
		/* default initialize thread attribute */
		if(0 != pthread_attr_init(&attr)) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s:%d] pthread_attr_init", __FUNCTION__, __LINE__);
			return REMOTE_PUSH_ERRNO_CREATE;
		}
		
		/* set detach attribute */
		if(0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s:%d] pthread_attr_setdetachstate", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return REMOTE_PUSH_ERRNO_CREATE;
		}
		
		/* set inherit schedual attribute */
		if(0 != pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s:%d] pthread_attr_setinheritsched", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return REMOTE_PUSH_ERRNO_CREATE;
		}

	    if(0 != pthread_attr_setstacksize(&attr, 32*1024)) {
		   dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "pthread_attr_setstacksize");
		   return REMOTE_PUSH_ERRNO_CREATE;
	   	}
		if(pthread_create(&gMsgTaskID, &attr, remotePushModuleMsgTask , NULL)){
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "serv: %s %d error and exit",__FUNCTION__ ,__LINE__);
			return REMOTE_PUSH_ERRNO_CREATE;
		}
	}
	return REMOTE_PUSH_ERRNO_SUCCESS;		

}

/*
 *Name: remotePushModuleFileTaskCreate
 *Brief:create a task to push file or big data to remote moblie App
 *Args:None
 *Return:UINT32(REMOTE_PUSH_ERRNO_SUCCESS or other errcodes)
 */
static remotePushErrno_e remotePushModuleFileTaskCreate()
{
	if(0 == gFileTaskID) {
		/*pthread setting*/
		pthread_attr_t attr;
		
		/* default initialize thread attribute */
		if(0 != pthread_attr_init(&attr)) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s:%d] pthread_attr_init", __FUNCTION__, __LINE__);
			return REMOTE_PUSH_ERRNO_CREATE;
		}
		
		/* set detach attribute */
		if(0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s:%d] pthread_attr_setdetachstate", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return REMOTE_PUSH_ERRNO_CREATE;
		}
		
		/* set inherit schedual attribute */
		if(0 != pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "[%s:%d] pthread_attr_setinheritsched", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return REMOTE_PUSH_ERRNO_CREATE;
		}

	    if(0 != pthread_attr_setstacksize(&attr, 32*1024)) {
		   dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "pthread_attr_setstacksize");
		   return REMOTE_PUSH_ERRNO_CREATE;
	   	}
		
		if(pthread_create(&gFileTaskID, &attr, remotePushModuleFileTask , NULL)){
			dbgLogPrint(REMOTE_PUSH_LVL_ERROR, "serv: %s %d error and exit",__FUNCTION__ ,__LINE__);
			return REMOTE_PUSH_ERRNO_CREATE;
		}
	}
	return REMOTE_PUSH_ERRNO_SUCCESS;		
}

/*
  *Name:remotePushModuleInit
  *Brief:Initialize the remote-pushing module
  *Args:None
  *Return:void
  */

remotePushErrno_e remotePushModuleInit()
{
	//tinyAIoT_DbgPrioritySet(TINYAIOT_LOG_DEBUG);
	dbgLogPrint(REMOTE_PUSH_LVL_WARN,"[%s:%d]version(0x%08x)!",__FUNCTION__,__LINE__, tinyAIoT_VersionNumberGet());
	remotePushErrno_e eRet = remotePushModuleIOInit();
	if(REMOTE_PUSH_ERRNO_SUCCESS != eRet) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:remotePushModuleIOInit fail!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_OTHERS;
		return eRet;
	}
	remotepushModuleTutkCompatibleModeGet(1);
	eRet = remotePushModuleMsgTaskCreate();
	if(REMOTE_PUSH_ERRNO_SUCCESS != eRet) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:remotePushModuleIOInit fail!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_OTHERS;
		goto EXIT;
	}
	eRet = remotePushModuleFileTaskCreate();
	if(REMOTE_PUSH_ERRNO_SUCCESS != eRet) {
		dbgLogPrint(REMOTE_PUSH_LVL_ERROR,"[%s:%d]error:remotePushModuleIOInit fail!",__FUNCTION__,__LINE__);
		eRet = REMOTE_PUSH_ERRNO_OTHERS;
		goto EXIT;
	}
	return REMOTE_PUSH_ERRNO_SUCCESS;
EXIT:
	remotePushModuleIODeinit();
	return eRet;
}

/*
  *Name:remotePushModuleDeInit
  *Brief:Denitialize the remote-pushing module
  *Args:None
  *Return:void
  */
remotePushErrno_e remotePushModuleDeInit()
{
	gLoopMsg = FALSE;
	gLoopFile = FALSE;
	gMsgTaskID = 0;
	gFileTaskID = 0;
	remotePushModuleIODeinit();
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

