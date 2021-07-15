
/***************************************************************************
 *Filename:awsiot_receiver.cpp
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
#include <unistd.h>
#include <sys/time.h>
#include "iotc_debug_print.h"
#include "iotc_receiver.h"
#include "iotc_console.h"
#include "iotc_io.h"
#include "libnlmci.h"
#include "icatchtek_mt7682_def.h"
/********************************************************************************/
/*                                     MACROS                                   */
/********************************************************************************/
using namespace iot::queue;

/********************************************************************************/
/*                             TYPE DEFINITION                                  */
/********************************************************************************/
#define WLAN_WIFI_HAND_CONNECT	"/proc/driver/icatchtek_wlan/connect"

/********************************************************************************/
/*                                  VARIABLES                                   */
/********************************************************************************/
static int ioHandle = -1;
static int ioInited = 0;

/********************************************************************************/
/*                                  FUNCTION DECLARATION                         */
/********************************************************************************/




/********************************************************************************/
/*                                  FUNCTION DEFINITION                         */
/********************************************************************************/
/*
 *Name: IotIoRead
 *Brief:Transmit data to MT7682 via SDIO.
 *Args:*Args:[void *]pData --- the pointer to the data to be sent.
 *[UINT32]dataSize --- the size of data to be sent.
 *Return:[IotErrNo_e]
 */
EXTERNC IotErrNo_e IotIoReceive(void *pBuf, UINT32 bufSize, UINT32 *pBytesRecv, UINT32 timeoutMs)
{
	/* parameter check */
	if(NULL == pBuf || 0 == bufSize) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] Parameter illegal", __FUNCTION__, __LINE__);
        return IOT_ERRNO_PARAM;
	}
	int type;
	int bytesRecv = 0;
	UINT32 retry = timeoutMs;
	do {
		bytesRecv = nlmci_recv(ioHandle, pBuf, bufSize, &type);
		if (bytesRecv < 0) {
			if (errno == ETIME) {
				continue;
			}
			break;
		}
		
		if ((type & 0xFFFF) != IOT_EVT_REQUEST_IOTC_SET) {
			continue;
		}
		else {
			break;
		}
		
		usleep(1000);	/* 1ms */
	}while(retry--);
	
	if(0 == retry) {
		return IOT_ERRNO_TIMEOUT;
	}
	*pBytesRecv = bytesRecv;
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotIoSend
 *Brief:Transmit data to MT7682 via SDIO.
 *Args:*Args:[void *]pData --- the pointer to the data to be sent.
 *[UINT32]dataSize --- the size of data to be sent.
 *Return:[IotErrNo_e]
 */
EXTERNC IotErrNo_e IotIoSend(void *pData, UINT32 dataSize)
{
	/* parameter check */
	if(NULL == pData || 0 == dataSize) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] Parameter illegal", __FUNCTION__, __LINE__);
        return IOT_ERRNO_PARAM;
	}
	
	UINT32 bufSize = dataSize;
	char *pBuf = (char*)malloc(bufSize);
	if(NULL == pBuf) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]allocate fail", __FUNCTION__, __LINE__);
		return IOT_ERRNO_MALLOC;
	}
	memcpy(pBuf, pData, dataSize);
	if(-1 == nlmci_send(ioHandle, (const void*)pBuf, (size_t)bufSize, IOT_CMD_REQUEST_IOTC_SET)) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] io Send fail", __FUNCTION__, __LINE__);
		free(pBuf);
		return IOT_ERRNO_SEND;
	}
	free(pBuf);
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotIoInit
 *Brief:Initialize IO.
 *Args:void
 *Return:[IotErrNo_e]
 */
EXTERNC IotErrNo_e IotIoInit()
{
	if(!ioInited) {
		dbgLogPrint(IOT_LVL_WARN, "[%s:%d] io init start", __FUNCTION__, __LINE__);
		ioHandle = nlmci_create();
		if(-1 == ioHandle) {
			ioInited = 0;
			dbgLogPrint(IOT_LVL_WARN, "[%s:%d] io init fail", __FUNCTION__, __LINE__);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		#if 0
		if(-1 == nlmci_send(ioHandle, IOT_READY_STR, strlen(IOT_READY_STR)+1, IOT_CMD_REQUEST_IOTC_SET)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] io Send fail", __FUNCTION__, __LINE__);
			return IOT_ERRNO_SEND;
		}
		#endif
		ioInited = 1;
		dbgLogPrint(IOT_LVL_WARN, "[%s:%d] io init done", __FUNCTION__, __LINE__);
		return IOT_ERRNO_SUCCESS;
	} else {
		dbgLogPrint(IOT_LVL_WARN, "[%s:%d] io has been inited", __FUNCTION__, __LINE__);
		return IOT_ERRNO_SUCCESS;
	}
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotIoDeinit
 *Brief:Deinitialize IO.
 *Args:void
 *Return:[IotErrNo_e]
 */

EXTERNC IotErrNo_e IotIoDeinit()
{
	#if 0
	if(-1 == nlmci_send(ioHandle, (void*)IOT_NOT_READY_STR, strlen(IOT_NOT_READY_STR)+1, IOT_CMD_REQUEST_IOTC_SET)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] io Send fail", __FUNCTION__, __LINE__);
			return IOT_ERRNO_SEND;
	}
	#endif
	if(-1 == nlmci_close(ioHandle)) {
		return IOT_ERRNO_UNKNOWN;
	}
	ioInited = 0;
	return IOT_ERRNO_SUCCESS;
}

