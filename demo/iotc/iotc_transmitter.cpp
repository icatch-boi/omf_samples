/***************************************************************************
 *Filename:Iotc_transmitter.cpp
 *Brief:
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
#include <pthread.h>
#include <unistd.h>

#include "iotc_debug_print.h"
#include "iotc_transmitter.h"
#include "iotc_io.h"

/********************************************************************************/
/*                                     MACROS                                   */
/********************************************************************************/
using namespace iot::queue;


/********************************************************************************/
/*                             TYPE DEFINITION                                  */
/********************************************************************************/



/********************************************************************************/
/*                                  VARIABLES                                   */
/********************************************************************************/
static pthread_t gDataTxTaskID = 0;
static UINT32 gTxLoop = 1;
/********************************************************************************/
/*                                  FUNCTION DECLARATION                         */
/********************************************************************************/

static IotErrNo_e IotDataTransmit(void *pData, UINT32 size);


/********************************************************************************/
/*                                  FUNCTION DEFINITION                         */
/********************************************************************************/
/*
 *Name: IotDataTransmit
 *Brief:Transmit data too MT7682 via SDIO.
 *Args:[void*] pData --- pointer to the data to be sent.
 *[UINT32]size ---the size of data to be sent.
 *Return:[IotErrNo_e]
 */
static IotErrNo_e IotDataTransmit(void *pData, UINT32 size)
{
	if(NULL == pData || 0 == size) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] Parameter illegal", __FUNCTION__, __LINE__);
        return IOT_ERRNO_PARAM;
	}
	if(IOT_ERRNO_SUCCESS != IotIoSend(pData, size)) {
		return IOT_ERRNO_SEND;
	}
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotTxTask
 *Brief:Task to transmit data to remote.
 *Args:[void *]arg
 *Return:[void*]
 */
 void* IotTxTask(void *arg)
{
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]IotTxTask start", __FUNCTION__,__LINE__);
	while(gTxLoop) {
		IotSession_t *pSessList = IotSessionList();
		if(NULL == pSessList) {
			usleep(10*1000);
			continue;
		}
		
		int idx = 0;
		for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
			IotSession_t *pSess = &pSessList[idx];
			if(NULL != pSess && 0 != strlen(pSess->remoteID)) {
				if(NULL != pSess->queueAttr.txQueue) {
					/* dequeue */
					IotQueueElem elemTxQ;
					dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]", __FUNCTION__,__LINE__);
					if(false == pSess->queueAttr.txQueue->Wait_dequeue_for(elemTxQ, 1000*1000)){/* return immediately if queue is empty */
						continue;
					}
		
					/* data check */
					void *pData = elemTxQ.DataBuf();
					UINT32 dataSize = elemTxQ.DataSize();
					dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]pData(%p)size(%d)", __FUNCTION__,__LINE__,pData, dataSize);
					if((NULL != pData) && (0 != dataSize)) {
						dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]", __FUNCTION__,__LINE__);
						
						/* fill header */
						IotTxDataHeader_t header = {0};
						UINT32 headerSize = sizeof(IotTxDataHeader_t);
						header.size = dataSize + headerSize;
						strcpy(header.remoteID, pSess->remoteID);
						UINT32 bufSize = headerSize + dataSize;	
						char *pBuf = (char*)malloc(bufSize);/* buffer layout: header|data payload */
						if(NULL == pBuf) {
							continue;
						}
						
						memset(pBuf, 0, bufSize);
						memcpy(pBuf, &header, headerSize);
						memcpy(pBuf+headerSize, pData, dataSize);
						/* send to mt7682 via sdio */
						IotDataTransmit(pBuf, bufSize);	
					}			
				}
			}
		}
		usleep(10*1000);
	}
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]appIotTxTask end", __FUNCTION__,__LINE__);
	gDataTxTaskID = 0;
	return NULL;
}
/*
 *Name: IotTxTaskCreate
 *Brief:create a task to transmit data to remote.
 *Args:void
 *Return:[IotErrNo_e]
 */
EXTERNC IotErrNo_e IotTxTaskCreate()
{
	if(0 == gDataTxTaskID) {
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d] IotTxTaskCreate start", __FUNCTION__, __LINE__);
		pthread_attr_t attr;
		
		/* default initialize thread attribute */
		if(0 != pthread_attr_init(&attr)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotTxTaskCreate::pthread_attr_init", __FUNCTION__, __LINE__);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* set detach attribute */
		if(0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotTxTaskCreate::pthread_attr_setdetachstate", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* set inherit schedual attribute */
		if(0 != pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotTxTaskCreate::pthread_attr_setinheritsched", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* create task */
		if(0 != pthread_create(&gDataTxTaskID, &attr,&IotTxTask, NULL)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotTxTaskCreate::pthread_create", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* destroy attribute */
		pthread_attr_destroy(&attr);
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d] IotTxTaskCreate end", __FUNCTION__, __LINE__);
	}	
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotTxTaskDestroy
 *Brief:destroy a task ehich is to transmit data to remote.
 *Args:void
 *Return:[IotErrNo_e]
 */
EXTERNC IotErrNo_e IotTxTaskDestroy()
{
	if(0 != gDataTxTaskID) {
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d] IotTxTaskDestroy", __FUNCTION__, __LINE__);
		gTxLoop = 0;
	}	
	return IOT_ERRNO_SUCCESS;
}


