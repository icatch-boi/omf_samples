
/***************************************************************************
 *Filename:iot_console.cpp
 *Brief:Provide a series of interfaces to use IoT communication mechanism.
 ***************************************************************************/

/********************************************************************************/
/*                                 DEBUG LOG CONTROL                            */
/********************************************************************************/
#define IOT_PRINT_LOG_EN	1
#define IOT_DEBUG_DEF_LEVEL		IOT_LVL_DEBUG

/********************************************************************************/
/*                                  HEADER FILES                                */
/********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "iotc_debug_print.h"
#include "iotc_console.h"
#include "iotc_transmitter.h"
#include "iotc_receiver.h"
#include "iotc_io.h"
#include "iotc_listen.h"

/********************************************************************************/
/*                                     DECLARATION                              */
/********************************************************************************/
using namespace iot::queue;

/********************************************************************************/
/*                                     MACROS                                   */
/********************************************************************************/

/********************************************************************************/
/*                             TYPE DEFINITION                                  */
/********************************************************************************/

/********************************************************************************/
/*                                  VARIABLES                                   */
/********************************************************************************/
static IotSession_t *gSessList = NULL;
static UINT8 gIotInited = 0;
/********************************************************************************/
/*                                  FUNCTION DECLARATION                         */
/********************************************************************************/
IotErrNo_e IotLoopTaskCreate(void *param);
IotErrNo_e IotLoopTaskDestroy(void *param);


/********************************************************************************/
/*                                  FUNCTION DEFINITION                         */
/********************************************************************************/

/*
 *Name: IotReadyInform
 *Brief:Inform MT7682 that IOTC of V37 is ready or not.
 *Args:void
 *Return:[IotErrNo_e]
 */
static IotErrNo_e IotReadyInform(IotReady_t state)
{
	if(IOT_READY == state) {
		IotIoSend((void*)IOT_READY_STR, strlen(IOT_READY_STR)+1);
	}
	else if(IOT_NOT_READY == state) {
		IotIoSend((void*)IOT_NOT_READY_STR, strlen(IOT_NOT_READY_STR)+1);
	}
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotInit
 *Brief:Initialize IOT communication mechanism.Only initialize once.
 *Args:void
 *Return:[IotErrNo_e]
 */
EXTERNC IotErrNo_e IotInit()
{
	if(0 == gIotInited) {
		/* create session list */
		UINT32 listSize = IOT_SESSION_MAX_NUM * sizeof(IotSession_t);
		gSessList = (IotSession_t *)malloc(listSize);
		if(NULL == gSessList) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] allocate memory for session list error", __FUNCTION__, __LINE__);
	        return IOT_ERRNO_MALLOC;
		}
		memset(gSessList, 0, listSize);
		
		/* init io */
		IotIoInit();
		
		/* create Tx task */
		if(IOT_ERRNO_SUCCESS != IotTxTaskCreate()) {
			return IOT_ERRNO_CONDITION;
		}
		/* create Rx task */
		if(IOT_ERRNO_SUCCESS != IotRxTaskCreate()) {
			return IOT_ERRNO_CONDITION;
		}
		
		/* inform MT7682 that IOTC is ready */
		IotReadyInform(IOT_READY);
		gIotInited = 1;
	}
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotDeinit
 *Brief:Denitialize IOT communication mechanism.Only deinitialize once.
 *Args:void
 *Return:[IotErrNo_e]
 */

EXTERNC IotErrNo_e IotDeinit()
{
	if(1 == gIotInited) {
		/* inform MT7682 that IOTC is not ready */
		IotReadyInform(IOT_NOT_READY);
		
		/* destroy Tx task */
		if(IOT_ERRNO_SUCCESS != IotTxTaskDestroy()) {
			return IOT_ERRNO_DESTROY_OBJECT;
		}
		
		/* destroy session list */
		int idx = 0;
		for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
			if(0 != strlen(gSessList[idx].remoteID)) {
				IotDestroySession(&gSessList[idx]);	
			}
		}
		if(NULL != gSessList) {
			free((void*)gSessList);
			gSessList = NULL;
		}
		IotIoDeinit();
		gIotInited  = 0;
	}
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotInitSession
 *Brief:initialize the specific session with default parameters.
 *Args:[IotSession_t *]pSess --- pointer to the target object of IotSession_t.
 *Return:[IotErrNo_e]
 */
static IotErrNo_e IotInitSession(IotSession_t *pSess)
{
	dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d] IotInitSession start", __FUNCTION__, __LINE__);
	/* check parameters  */
	if(NULL == pSess) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] target session is NULL", __FUNCTION__, __LINE__);
		return IOT_ERRNO_PARAM;
	}

	/* check session ID */
	if(!(pSess->remoteID && strlen(pSess->remoteID))) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] remote ID of session error", __FUNCTION__, __LINE__);
		return IOT_ERRNO_PARAM;
	}

	/* create Rx queue of session */
	if(NULL == pSess->queueAttr.rxQueue) {
		pSess->queueAttr.rxQueue = new IotBlockingQueue;
		if(NULL == pSess->queueAttr.rxQueue) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] create Rx queue for channel error", __FUNCTION__, __LINE__);
			return IOT_ERRNO_CREATE_OBJECT;
		}
	}
	
	/* create Tx queue of session */
	if(NULL == pSess->queueAttr.txQueue) {
		pSess->queueAttr.txQueue = new IotBlockingQueue;
		if(NULL == pSess->queueAttr.txQueue) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] create Tx queue for channel error", __FUNCTION__, __LINE__);
			return IOT_ERRNO_CREATE_OBJECT;
		}
	}
	dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d] IotInitSession end", __FUNCTION__, __LINE__);
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotCreateSession
 *Brief:Create an object of IotSession_t specified by the session ID.
 *Args:
 *[const char*]remoteID --- remoteID of session.
 *[IotSession_t **]ppSess --- pointer to the destination object of IotSession_t.
 *Return:[IotErrNo_e]
 */

EXTERNC IotErrNo_e IotCreateSession(const char *remoteID, IotSession_t **ppSess)
{
	/* check parameters */
	if(NULL==remoteID || NULL==ppSess) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] param error", __FUNCTION__, __LINE__);
		return IOT_ERRNO_PARAM;
	}
	
	UINT32 remoteIDLen = strlen(remoteID)+1;
	if(0==remoteIDLen || IOT_REMOTE_ID_MAX_LENGTH<remoteIDLen) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] the string length of clientID is much too short or long", __FUNCTION__, __LINE__);
		return IOT_ERRNO_PARAM;
	}

	/* check session list */
	if(NULL == gSessList) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] please call IotInit firstly", __FUNCTION__, __LINE__);
		return IOT_ERRNO_CONDITION;
	}
	
	/* session already existed? */
	int idx = 0;
	for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
		if(0 == strcmp(remoteID, gSessList[idx].remoteID)) {
			dbgLogPrint(IOT_LVL_INFO, "[%s:%d] session aleady existed,ID(%s)", __FUNCTION__, __LINE__, remoteID);
			*ppSess = &gSessList[idx];
			return IOT_ERRNO_SUCCESS;
		}
	}
	
	/* add new session */
	for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {/* ID is the idx of channel in list, ID=0 is reserved */
		if(0 == strlen(gSessList[idx].remoteID)) {
			break;
		}
	}
	/* exceed max session number */
	if(IOT_SESSION_MAX_NUM <= idx) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] exceed max session limitation(%d)", __FUNCTION__, __LINE__, IOT_SESSION_MAX_NUM);
		return IOT_ERRNO_EXCEED_MAX;
	}
	memcpy(gSessList[idx].remoteID, remoteID, remoteIDLen);
	/* initialize session */
	if(IOT_ERRNO_SUCCESS != IotInitSession(&gSessList[idx])) {
		return IOT_ERRNO_UNKNOWN;
	}

	*ppSess = &gSessList[idx];
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d] create session ok,ID(%s)", __FUNCTION__, __LINE__, (*ppSess)->remoteID);
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotDestroySession
 *Brief:Destroy the session object specified by the pointer.
 *Args:[IotSession_t *]pSess --- pointer to the object of IotSession_t which is to be destroied.
 *Return:[IotErrNo_e]
 */

EXTERNC IotErrNo_e IotDestroySession(IotSession_t *pSess)
{
	/* check parameter */
	if(NULL == pSess) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] the session to be destroied is NULL", __FUNCTION__, __LINE__);
		return IOT_ERRNO_PARAM;
	}
	
	/* check session list */
	if(NULL == gSessList) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] please invoke IotInit firstly", __FUNCTION__, __LINE__);
		return IOT_ERRNO_CONDITION;
	}
	
	/* search session list for session */
	int idx = 0;
	for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
		if(pSess == &gSessList[idx]) {
			break;
		}
	}
	if(IOT_SESSION_MAX_NUM <= idx) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] not find the session in session list", __FUNCTION__, __LINE__);
		return IOT_ERRNO_PARAM;
	}
	
	/* destroy Rx queue */
	if(NULL != pSess->queueAttr.rxQueue) {
		delete pSess->queueAttr.rxQueue;
		pSess->queueAttr.rxQueue = NULL;
	}	

	/* destroy Tx queue */
	if(NULL != pSess->queueAttr.txQueue) {
		delete pSess->queueAttr.txQueue;
		pSess->queueAttr.txQueue = NULL;
	}	
	
	/* remove from session list */
	memset(pSess, 0, sizeof(IotSession_t));
	
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotRead
 *Brief:Read data via the specific channel.
 *Args:
 *[const IotSession_t *]pSess --- the pointer to the owner of channel.
 *[void*]pBuffer --- buffer to receive data.
 *[UINT32]bufSize --- the size of buffer.
 *[UINT32]timeoutMs --- timeout in ms.
 *Return:[IotErrNo_e]
 */

EXTERNC IotErrNo_e IotRead(const IotSession_t *pSess, void *pBuffer, UINT32 bufSize, UINT32 *pReadSize, UINT32 timeoutMs)
{
	/* check parameter */
	if(NULL == pSess) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] the session is NULL", __FUNCTION__, __LINE__);
		return IOT_ERRNO_PARAM;
	}
	
	if(NULL == pBuffer || 0 == bufSize) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] buffer parameters illegal", __FUNCTION__, __LINE__);
        return IOT_ERRNO_PARAM;
	}
	
	/* check Rx queue  */
	if(NULL == pSess->queueAttr.rxQueue) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] Rx queue not exist", __FUNCTION__, __LINE__);
		return IOT_ERRNO_CONDITION;
	}
	
	/* dequeue */
	IotQueueElem elemRxQ;
	if(false == pSess->queueAttr.rxQueue->Wait_dequeue_for(elemRxQ, timeoutMs*1000)) {
		return IOT_ERRNO_UNKNOWN;
	}

	/* copy data to destination buffer */
	elemRxQ.DataCopy(pBuffer, bufSize);
	*pReadSize = elemRxQ.DataSize();
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotWrite
 *Brief:Write data via the specific channel.
 *Args:[const IotSession_t *]pSess --- the pointer to the object of session.
 *[void*]pData --- pointer to the data.
 *[UINT32]dataSize --- the size of data.
 *[UINT32]timeoutMs --- timeout in ms.
 *Return:[IotErrNo_e]
 */

EXTERNC IotErrNo_e IotWrite(const IotSession_t *pSess, const void *pData, UINT32 dataSize, UINT32 timeoutMs)
{
	/* check parameter */
	if(NULL == pSess) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] the owner of channel is NULL", __FUNCTION__, __LINE__);
		return IOT_ERRNO_PARAM;
	}
	
	if(NULL == pData || 0 == dataSize) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] data parameters illegal", __FUNCTION__, __LINE__);
        return IOT_ERRNO_PARAM;
	}
	
	/* check Tx queue  */
	if(NULL == pSess->queueAttr.txQueue) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] Tx queue not exist", __FUNCTION__, __LINE__);
		return IOT_ERRNO_CONDITION;
	}
	
	/* copy data to a heap memory */
	UINT32 bufSize = dataSize;
	char *pBuf = (char*)malloc(bufSize);
	if(NULL == pBuf) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] malloc fail", __FUNCTION__, __LINE__);
		return IOT_ERRNO_MALLOC;
	}

	memset(pBuf, 0, dataSize);
	memcpy(pBuf, pData, dataSize);
	
	/* fill element */
	IotQueueElem elemTxQ;
	elemTxQ.DataBufBind(pBuf);
	elemTxQ.DataSize(bufSize);
	dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]pData(%p)size(%d)", __FUNCTION__,__LINE__,pBuf, bufSize);
	std::function<bool(void**)> release = [](void** pBuf) {
		dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]pData(%p)", __FUNCTION__,__LINE__,*pBuf);
		if(!(*pBuf)) {
			return false;
		}
		free(*pBuf);
		*pBuf = NULL;
		return true;
	};
	elemTxQ.ReleaseMethodBind(release);
	
	/*enqueue*/
	UINT32 retry = timeoutMs;
	while(retry--) {
		if(true == pSess->queueAttr.txQueue->Enqueue(elemTxQ)) {
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
 *Name: IotSessionList
 *Brief:Get the pointer to session list.
 *Args:void.
 *Return:[IotSessQueueAttr_t]pointer to session list.
 */

EXTERNC IotSession_t * IotSessionList()
{	
	return gSessList;
}
