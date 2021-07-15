#ifndef _TINYAIOT_IOT_API_H_
#define _TINYAIOT_IOT_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tinyaiot_error_code.h"

#define MAX_LENGTH_OF_UPDATE_JSON_BUFFER 8000 //JSON limit 8kB
#define DEFAULT_NUMBER_CONNECTIONS 3

//typedef tinyaiotErrorCode_e (*IotWakeupCallbackFunction_t)(const int status);
typedef tinyaiotErrorCode_e (*IotCmdCallbackFunction_t)(const char* remoteId, const char *pRecvData, unsigned int recvLen, void *param);
typedef tinyaiotErrorCode_e (*IotStateCallbackFunction_t)(const char *pRecvData, unsigned int recvLen, void *param);
typedef tinyaiotErrorCode_e (*IotDeadCallbackFunction_t)(int reason, void *param);

typedef tinyaiotErrorCode_e (*IotConnectedCallbackFunction_t)(const char* remoteId, const char *pRecvData, unsigned int recvLen, void *param);
typedef tinyaiotErrorCode_e (*IotDisconnectedCallbackFunction_t)(const char* remoteId, unsigned int connectedNum, void *param);

typedef struct _TinyAIoT_IotInitParameters_t{
	char *pHost;
	int port;
	enum {AWSIOT, ALIIOT} type;  /* NOTES: ALIIOT Not Supported. */
	union {
		struct {
			char *pRootCA; /* String Buffer of the Root CA */
			char *pClientCRT; /* String Buffer of Device certs signed by AWS IoT service */
			char *pClientKey; /* String Buffer of Device private key */
			char *thingsname;
		}aws; /* for type is AWSIOT */
		struct {
			char *ProductSecret;
			char *pProductKey;
			char *pDeviceName;
			char *pDeviceSecret;
		}ali;  /* for type is AWSIOT */
	}certificate;

	IotCmdCallbackFunction_t cmdHandler;
	void *cmdHandlerData;          /* Data to pass as argument when command handler is called */

	int keepAlive; /* 0 is disable, else enable. */
	IotDeadCallbackFunction_t deadHandler; /*  Callback to be invoked upon keep alive loss */
	void *deadHandlerData;          /* Data to pass as argument when disconnect handler is called */

	int maxConnections; /* Maximum number of connections for remote application */
	IotConnectedCallbackFunction_t connectedHandler;
	void *connectedHandlerData;          /* Data to pass as argument when connected handler is called */
	IotDisconnectedCallbackFunction_t disconnectedHandler;
	void *disconnectedHandlerData;          /* Data to pass as argument when disconnected handler is called */
}TinyAIoT_IotInitParameters_t;

/**
 * \brief Initialize IoT module.
 *
 * \details
 *
 * \param pParam[in] Initialize parameters, contain:
 *        pParam->pHost[in] Required, Service address for iot supporter, it's ignored when base on subiot slave.
 *        pParam->port[in]  Required, Service port for iot supporter, it's ignored when base on subiot slave.
 *        pParam->type[in]  Required, Sub iot supporter type.
 *        pParam->certificate[in]  Required, Security credential for iot supporter,
 *                                    it's optional except "pParam->certificate.aws.thingsname"/"pParam->certificate.ali.pDeviceName" when base on subiot slave.
 *
 *        pParam->cmdHandler[in]     Optional, User-defined callback function for receiving message by command channel.
 *        pParam->cmdHandlerData[in] Optional, User-defined private data for cmdHandler function.
 *        pParam->keepAlive[in]       Optional, Only enable support.
 *        pParam->deadHandler[in]     Optional, User-defined callback function for receiving keep alive loss notify, this available when keepAlive enable.
 *        pParam->deadHandlerData[in] Optional, User-defined private data for deadHandler function.
 *        pParam->maxConnections[in]          Optional, User-defined maximum connectable clients, default DEFAULT_NUMBER_CONNECTIONS.
 *        pParam->connectedHandler[in]        Optional, User-defined callback function for new client connected.
 *        pParam->connectedHandlerData[in]    Optional, User-defined private data for connectedHandler.
 *        pParam->disconnectedHandler[in]     Optional, User-defined callback function for client disconnected.
 *        pParam->disconnectedHandlerData[in] Optional, User-defined private data for disconnectedHandler.
 *
 * \return TINYAIOT_SUCCEED or ErrorCode, reference to tinyaiotErrorCode_e.
 *
 * \attention (1) This function must be called before any others interface, and should be called once only.
 * \attention (2) NEVER free pParam UNTIL DeInitialize IoT module by TinyAIoT_IotDeInitialize!
 */
tinyaiotErrorCode_e TinyAIoT_IotInitialize(TinyAIoT_IotInitParameters_t *pParam);
/**
 * \brief DeInitialize IoT module.
 *
 * \details
 *
 * \return TINYAIOT_SUCCEED or ErrorCode, reference to tinyaiotErrorCode_e.
 *
 * \attention (1) This function will destory any thread create by IoT Service, and release all resources.
 */
tinyaiotErrorCode_e TinyAIoT_IotDeInitialize(void);

/**
 * \brief Send message to remote client by event channel synchronously.
 *
 * \details Device want to send event message to client, please call it.
 *
 * \param remoteId[in]   Remote client id received by connectedHandler.
 * \param message[in]    Optional, event message, allow send null message to client.
 * \param timeoutSec[in] Optional, timeout, if 0, just try once.
 *
 * \return TINYAIOT_SUCCEED or ErrorCode, reference to tinyaiotErrorCode_e.
 */
tinyaiotErrorCode_e TinyAIoT_IotEventUpdate(char* remoteId, char *message, int timeoutSec);
/**
 * \brief Send message to remote client by response channel synchronously.
 *
 * \details Device want to send event message to client, please call it.
 *
 * \param remoteId[in]   Remote client id received by connectedHandler.
 * \param message[in]    Optional, response message, allow send null message to client.
 * \param timeoutSec[in] Optional, timeout, if 0, just try once.
 *
 * \return TINYAIOT_SUCCEED or ErrorCode, reference to tinyaiotErrorCode_e.
 */
tinyaiotErrorCode_e TinyAIoT_IotCmdResponseStatusUpdate(char* remoteId, char* message, int timeoutSec);

/**
 * \brief Report device state to IoT Service synchronously.
 *
 * \details Device want to report current state to IoT Service, please call it.
 *
 * \param jsonMessage[in]  Report state information, must JSON Format String, and some key is reserved like as "res"/"event"/"connected", maximum key number is 30.
 * \param timeoutSec[in]   Optional, timeout, if 0, just try once.
 *
 * \return TINYAIOT_SUCCEED or ErrorCode, reference to tinyaiotErrorCode_e.
 */
tinyaiotErrorCode_e TinyAIoT_IotStatusUpdate(char *jsonMessage, int timeoutSec);

#ifdef __cplusplus
}
#endif

#endif
