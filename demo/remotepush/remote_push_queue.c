/*
 *File Name:remote_push_queue.c
 *Brief:supply a series of interface of queue
 *Date:2020-08-05
 */



/**************************************************************************
 *                           I N C L U D E S	                           *
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "remote_push_errno.h"
#include "remote_push_queue.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
typedef unsigned int UINT32;
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/ 
 TAILQ_HEAD(queHead, queNode);	/* key statement */


/**************************************************************************
 *                           GLOBAL VARIABLES                             *
 **************************************************************************/
 
/**************************************************************************
 *                           FUNCTION DECLARATION                           *
 **************************************************************************/


/**************************************************************************
 *                           FUNCTION DEFINITION                           *
 **************************************************************************/


remotePushErrno_e remotePushQueueCreate(remotePushQueue **ppQueue)
{
	struct queHead *pQueHead = (struct queHead*)malloc(sizeof(struct queHead));
	if(NULL == pQueHead) {
		return 	REMOTE_PUSH_ERRNO_NOMEM;
	}
	TAILQ_INIT(pQueHead);
	*ppQueue = (remotePushQueue *)pQueHead;
	
	return REMOTE_PUSH_ERRNO_SUCCESS;
}


remotePushErrno_e remotePushQueueSend(const remotePushQueue *pQueue, remotePushQueueNode_t *pNode)
{
	/* verify parameters */
	if(NULL == pQueue || NULL == pNode) {
		return REMOTE_PUSH_ERRNO_PARAM;
	}
	
	/* insert the new element to the tail of queue */
	struct queHead *pQueHead = (struct queHead *)pQueue;
	TAILQ_INSERT_TAIL(pQueHead, pNode, next);
	
	return REMOTE_PUSH_ERRNO_SUCCESS;
}


remotePushErrno_e remotePushQueueReceive(const remotePushQueue *pQueue, remotePushQueueNode_t **ppNode, UINT32 timeoutMs)
{
	/* verify parameters */
	if(NULL == ppNode) {
		return REMOTE_PUSH_ERRNO_PARAM;
	}

	/* receive the oldest element */
	struct queHead *pQueHead = (struct queHead *)pQueue;
	UINT32 waitCount = timeoutMs/20;
	do {
		*ppNode = TAILQ_FIRST(pQueHead);
		if(NULL != *ppNode) {
			break;
		}

		if((0 == waitCount--) && (NULL == *ppNode)) {
			return REMOTE_PUSH_ERRNO_EMPTY;
		}
		usleep(20000);
	} while(1);
	
	/* remove this element from queue */
	TAILQ_REMOVE(pQueHead, *ppNode, next);
	
	return REMOTE_PUSH_ERRNO_SUCCESS;
}


remotePushErrno_e remotePushQueueDestroy(remotePushQueue **ppQueue)
{
	/* verify parameters */
	
	if(NULL == ppQueue || NULL == *ppQueue) {
		return REMOTE_PUSH_ERRNO_PARAM;
	}
	remotePushQueue *pQueue = *ppQueue;
	
	/* remove all elements */
	struct queHead *pQueHead = (struct queHead *)pQueue;
	
	remotePushQueueNode_t *pNode = TAILQ_FIRST(pQueHead);
	while(NULL != pNode) {
		TAILQ_REMOVE(pQueHead, pNode, next);
	}
	
	/* release memory of element */
	free(pNode);
	pNode = NULL;

	/* free memory of head of queue */
    free(pQueue);
	*ppQueue = NULL;
	
	return REMOTE_PUSH_ERRNO_SUCCESS;
}

