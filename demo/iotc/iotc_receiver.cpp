
/***************************************************************************
 *Filename:awsiot_receiver.cpp
 *Brief:
 ***************************************************************************/

/********************************************************************************/
/*                                 DEBUG LOG CONTROL                            */
/********************************************************************************/

#define IOT_PRINT_LOG_EN	1
#define IOT_DEBUG_DEF_LEVEL		IOT_LVL_INFO


/********************************************************************************/
/*                                  HEADER FILES                                */
/********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "iotc_debug_print.h"
#include "iotc_io.h"
#include "iotc_receiver.h"
#include "iotc_console.h"
#include "iotc_queue.h"
#include "iotc_listen.h"
/********************************************************************************/
/*                                     MACROS                                   */
/********************************************************************************/
using namespace iot::queue;
#define RX_BUFFER_SIZE   2048
/********************************************************************************/
/*                             TYPE DEFINITION                                  */
/********************************************************************************/


/********************************************************************************/
/*                                  VARIABLES                                   */
/********************************************************************************/
static pthread_t gDataRxTaskID = 0;
static UINT32 gRxLoop = 1;

/********************************************************************************/
/*                                  FUNCTION DECLARATION                         */
/********************************************************************************/




/********************************************************************************/
/*                                  FUNCTION DEFINITION                         */
/********************************************************************************/
/*
*Name: IotDataProc
*Brief:An internal function to receive data from MT7682.
*Args:[void*]pData ---pointer to the received data.
*[UINT32]dataSize---the size of the received data.
*[UINT32] timeoutMs --- timeout in ms.
*Return:[IotErrNo_e]
*/

static IotErrNo_e IotDataProc(void *pData, UINT32 dataSize, UINT32 timeoutMs)
{
	/* layout: header | data payload */
	if(NULL == pData || 0 == dataSize) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] Parameter illegal", __FUNCTION__, __LINE__);
        return IOT_ERRNO_PARAM;
	}

	/* search session list for session with the name of session */
	IotSession_t *pSessList =  IotSessionList();
	if(NULL == pSessList) {
		return IOT_ERRNO_CONDITION;
	}
	IotRxDataHeader_t *pRxHeader = (IotRxDataHeader_t *)pData;
	char *remoteID = pRxHeader->remoteID;

	if(NULL == remoteID) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] session ID error", __FUNCTION__, __LINE__);
        return IOT_ERRNO_PARAM;
	}
	/* search session list for session with remoteID */
	int idx = 0;
	int trycount = 5;
	while(1) {
		for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
			if(0 == strcmp(remoteID, pSessList[idx].remoteID)) {
				break;
			}
		}

		if(IOT_SESSION_MAX_NUM <= idx) {
			usleep(10*1000);
		}
		else {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] retry(%d)", __FUNCTION__, __LINE__, trycount);
			break;
		}

		if(0 == trycount) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] session not exist", __FUNCTION__, __LINE__);
			return IOT_ERRNO_PARAM;
		}
		trycount--;
	}

	
	IotSession_t *pSess = &pSessList[idx];
	if(NULL == pSess) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] session not exist", __FUNCTION__, __LINE__);
		return IOT_ERRNO_UNKNOWN;
	}
	
	if(NULL == pSess->queueAttr.rxQueue) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] session Rx queue error", __FUNCTION__, __LINE__);
		return IOT_ERRNO_CONDITION;
	}

	/* copy data to new memory */
	UINT32 bufSize = dataSize-sizeof(IotRxDataHeader_t);
	char* pDataPayload = (char*)pData+sizeof(IotRxDataHeader_t);
	
	char *pBuf = (char*)malloc(bufSize);
	if(NULL == pBuf) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] malloc error", __FUNCTION__, __LINE__);
		return IOT_ERRNO_MALLOC;
	}
	memset(pBuf, 0, bufSize);
	memcpy(pBuf, pDataPayload, bufSize);
	
	/* fill element */
	IotQueueElem  elemRxQ;
	elemRxQ.DataBufBind(pBuf);
	elemRxQ.DataSize(bufSize);
	std::function<bool(void**)> release = [](void** pBuf) {
		dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]pData(%p)", __FUNCTION__,__LINE__,*pBuf);
		if(!(*pBuf)) {
			return false;
		}
		free(*pBuf);
		*pBuf = NULL;
		return true;
	};
	elemRxQ.ReleaseMethodBind(release);

	/*enqueue*/
	UINT32 retry = timeoutMs;
	while(retry--) {
		if(true == pSess->queueAttr.rxQueue->Enqueue(elemRxQ)) {
			break;
		}
		usleep(1000);	/* 1ms */
	}

	if(0 == retry) {
		return IOT_ERRNO_TIMEOUT;
	}
	
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: appIotIOEvtCB
 *Brief:A callback function registered to receive data from MT7682.
 *Args:[void*]pData ---pointer to the received data.
 *[UINT32]dataSize---the size of the received data.
 *[UINT32] timeoutMs --- timeout in ms.
 *Return:[IotErrNo_e]
 */

EXTERNC IotErrNo_e IotIOEvt(void *pData, UINT32 dataSize, UINT32 timeoutMs)
{
	/* layout: header | data payload */
	if(NULL == pData || 0 == dataSize) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] Parameter illegal", __FUNCTION__, __LINE__);
        return IOT_ERRNO_PARAM;
	}
	
	/* search session list for session with the name of session */
	IotSession_t *pSessList =  IotSessionList();
	if(NULL == pSessList) {
		return IOT_ERRNO_CONDITION;
	}
	IotRxDataHeader_t *pRxHeader = (IotRxDataHeader_t *)pData;
	char *remoteID = pRxHeader->remoteID;

	if(NULL == remoteID) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] session ID error", __FUNCTION__, __LINE__);
        return IOT_ERRNO_PARAM;
	}
	/* search session list for session with remoteID */
	int idx = 0;
	for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
		if(0 == strcmp(remoteID, pSessList[idx].remoteID)) {
			break;
		}
	}
	
	if(IOT_SESSION_MAX_NUM <= idx) {
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d] session not exist, create it, ID(%s)", __FUNCTION__, __LINE__, remoteID);
        IotListenCB(remoteID);
	}

	IotDataProc(pData, dataSize, timeoutMs);
	return IOT_ERRNO_SUCCESS;
}




/*
 *Name: IotDataReceive
 *Brief:receive data from MT7682 via SDIO.
 *Args:[void*] pBuf --- pointer to the buffer.
 *[UINT32]size ---the size of buffer.
 *Return:[IotErrNo_e]
 */
static IotErrNo_e IotDataReceive(void *pBuf, UINT32 bufSize, UINT32 *pBytesRecv, UINT32 timeoutMs)
{
	if(NULL == pBuf || 0 == bufSize) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] Parameter illegal", __FUNCTION__, __LINE__);
        return IOT_ERRNO_PARAM;
	}
	return IotIoReceive(pBuf, bufSize, pBytesRecv, timeoutMs);
}

/*
 *Name: IotRxTask
 *Brief:Task to transmit data to remote.
 *Args:[void *]arg
 *Return:[void*]
 */
 void* IotRxTask(void *arg)
{
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]IotRxTask start", __FUNCTION__,__LINE__);
	UINT32 bufferSize = RX_BUFFER_SIZE;
	char *pBuffer = (char*)malloc(bufferSize);
	if(NULL == pBuffer) {
		return NULL;
	}
	while(gRxLoop) {
		memset(pBuffer, 0, bufferSize);
		uint32_t bytesRecv = 0;
		if(IOT_ERRNO_SUCCESS == IotDataReceive(pBuffer, bufferSize, &bytesRecv, 0xFFFFFFFF)) {
			IotIOEvt(pBuffer, bytesRecv, 100);
		}
		usleep(20*1000);	
	}
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]appIotTxTask end", __FUNCTION__,__LINE__);
	gDataRxTaskID = 0;
	return NULL;
}
/*
 *Name: IotRxTaskCreate
 *Brief:create a task to transmit data to remote.
 *Args:void
 *Return:[IotErrNo_e]
 */
EXTERNC IotErrNo_e IotRxTaskCreate()
{
	if(0 == gDataRxTaskID) {
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d] IotRxTaskCreate start", __FUNCTION__, __LINE__);
		pthread_attr_t attr;
		
		/* default initialize thread attribute */
		if(0 != pthread_attr_init(&attr)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotRxTaskCreate::pthread_attr_init", __FUNCTION__, __LINE__);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* set detach attribute */
		if(0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotRxTaskCreate::pthread_attr_setdetachstate", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* set inherit schedual attribute */
		if(0 != pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotRxTaskCreate::pthread_attr_setinheritsched", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* create task */
		if(0 != pthread_create(&gDataRxTaskID, &attr,&IotRxTask, NULL)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotRxTaskCreate::pthread_create", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* destroy attribute */
		pthread_attr_destroy(&attr);
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d] IotRxTaskCreate end", __FUNCTION__, __LINE__);
	}	
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotRxTaskDestroy
 *Brief:destroy a task ehich is to transmit data to remote.
 *Args:void
 *Return:[IotErrNo_e]
 */
EXTERNC IotErrNo_e IotRxTaskDestroy()
{
	if(0 != gDataRxTaskID) {
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d] IotRxTaskDestroy", __FUNCTION__, __LINE__);
		gRxLoop = 0;
	}	
	return IOT_ERRNO_SUCCESS;
}

