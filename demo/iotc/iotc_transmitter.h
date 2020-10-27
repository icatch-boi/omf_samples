#ifndef __IOT_TRANSMITTER_H__
#define __IOT_TRANSMITTER_H__

/********************************************************************************/
/*                                  HEADER FILES                                */
/********************************************************************************/

#include "iotc_console.h"

/********************************************************************************/
/*                                     MACROS                                   */
/********************************************************************************/
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif



/********************************************************************************/
/*                                  TYPE DECLARATION                            */
/********************************************************************************/
typedef struct __attribute__ ((packed, aligned(4))){
	UINT32 size;	/* headerSize + dataSize */
	char remoteID[IOT_REMOTE_ID_MAX_LENGTH];
}IotTxDataHeader_t;






/********************************************************************************/
/*                                 FUNCTION DECLARATION                         */
/********************************************************************************/
EXTERNC IotErrNo_e IotTxTaskCreate();
EXTERNC IotErrNo_e IotTxTaskDestroy();




#endif

