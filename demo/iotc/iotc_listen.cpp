/**************************************************************************
 *
 *       Copyright (c) 2006-2020 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 **************************************************************************/

/***************************************************************************
 *Filename:app_awsiot_transmitter.c
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
#include <pthread.h>
#include <unistd.h>

#include "iotc_listen.h"
#include "iotc_debug_print.h"
#include "iotc_queue.h"

/********************************************************************************/
/*                                     MACROS                                   */
/********************************************************************************/
using iot::queue::IotBlockingQueue;
using iot::queue::IotQueueElem;
/********************************************************************************/
/*                             TYPE DEFINITION                                  */
/********************************************************************************/



/********************************************************************************/
/*                                  VARIABLES                                   */
/********************************************************************************/
static IotBlockingQueue *pListenQ = NULL;

/********************************************************************************/
/*                                  FUNCTION DECLARATION                         */
/********************************************************************************/



/********************************************************************************/
/*                                  FUNCTION DEFINITION                         */
/********************************************************************************/

/*
 *Name: IotListen
 *Brief:Listen connection from remote.
 *Args:
 *[char *] pRemoteID --- pointer to the remote ID.
 *[UINT32] timeOutMs --- timeout in ms.
 *Return:IotErrNo_e
 */

EXTERNC IotErrNo_e IotListen(char *pRemoteID, UINT32 timeOutMs)
{
	if(NULL == pRemoteID) {
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d]param error", __FUNCTION__,__LINE__);
		return IOT_ERRNO_PARAM;
	}
	
	/* create queue */
	if(NULL == pListenQ) {
		pListenQ = new IotBlockingQueue;
		if(NULL == pListenQ) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] create queue for listen error", __FUNCTION__, __LINE__);
			return IOT_ERRNO_CREATE_OBJECT;
		}
	}
	/* listen */
	IotQueueElem elemListenQ;
	if(false == pListenQ->Wait_dequeue_for(elemListenQ, timeOutMs*1000)) {
		return IOT_ERRNO_UNKNOWN;
	}
	/* copy session ID */
	char *pSrcCliID = (char*)elemListenQ.DataBuf();
	if(NULL == pSrcCliID) {
		return IOT_ERRNO_UNKNOWN;
	}
	UINT32 srcCliIDLen = elemListenQ.DataSize();
	memcpy(pRemoteID, pSrcCliID, srcCliIDLen);
	return IOT_ERRNO_SUCCESS;
}


/*
 *Name: IotListenCB
 *Brief:Trigger once connection.
 *Args:
 *[char *] pRemoteID --- pointer to the remote session ID.
 *Return:IotErrNo_e
 */

EXTERNC IotErrNo_e IotListenCB(const char *pRemoteID)
{
	if(NULL == pRemoteID) {
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d]param error", __FUNCTION__,__LINE__);
		return IOT_ERRNO_PARAM;
	}
	
	/* create queue */
	if(NULL == pListenQ) {
		pListenQ = new IotBlockingQueue;
		if(NULL == pListenQ) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] create queue for listen error", __FUNCTION__, __LINE__);
			return IOT_ERRNO_CREATE_OBJECT;
		}
	}
	/* listen */
	IotQueueElem elemListenQ;
	UINT32 remoteIDLen = strlen(pRemoteID);
	if(0==remoteIDLen) {
		return IOT_ERRNO_PARAM;
	}
	char *pSrcID = (char*)malloc(remoteIDLen);
	memcpy(pSrcID, pRemoteID, remoteIDLen);
	elemListenQ.DataBufBind(pSrcID);
	elemListenQ.DataSize(remoteIDLen);
	std::function<bool(void**)> release = [](void** pBuf) {
		dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]pData(%p)", __FUNCTION__,__LINE__,*pBuf);
		if(!(*pBuf)) {
			return false;
		}
		free(*pBuf);
		*pBuf = NULL;
		return true;
	};
	elemListenQ.ReleaseMethodBind(release);
	UINT32 retry = 100;
	while(retry--) {
		if(true == pListenQ->Enqueue(elemListenQ)) {
			break;
		}
		usleep(1000);	/* 1ms */
	}

	if(0 == retry) {
		return IOT_ERRNO_TIMEOUT;
	}
	return IOT_ERRNO_SUCCESS;
}

