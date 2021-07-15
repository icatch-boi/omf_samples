#ifndef TINYAIOT_AWSIOT_MQTT_CLIENT_INTERFACE_H
#define TINYAIOT_AWSIOT_MQTT_CLIENT_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* AWS Specific header files */
#include "aws_iot_error.h"
//#include "aws_iot_config.h"
#include "aws_iot_mqtt_client.h"

/**
 * @brief MQTT Client Initialization Function
 *
 * Called to initialize the MQTT Client
 *
 * @param pClient Reference to the IoT Client
 * @param pInitParams Pointer to MQTT connection parameters
 *
 * @return IoT_Error_t Type defining successful/failed API call
 */
IoT_Error_t tinyaiot_awsiot_mqtt_init(AWS_IoT_Client *pClient, IoT_Client_Init_Params *pInitParams);

/**
 * @brief MQTT Connection Function
 *
 * Called to establish an MQTT connection with the AWS IoT Service
 *
 * @param pClient Reference to the IoT Client
 * @param pConnectParams Pointer to MQTT connection parameters
 *
 * @return An IoT Error Type defining successful/failed connection
 */
IoT_Error_t tinyaiot_awsiot_mqtt_connect(AWS_IoT_Client *pClient, IoT_Client_Connect_Params *pConnectParams);

/**
 * @brief Publish an MQTT message on a topic
 *
 * Called to publish an MQTT message on a topic.
 * @note Call is blocking.  In the case of a QoS 0 message the function returns
 * after the message was successfully passed to the TLS layer.  In the case of QoS 1
 * the function returns after the receipt of the PUBACK control packet.
 *
 * @param pClient Reference to the IoT Client
 * @param pTopicName Topic Name to publish to
 * @param topicNameLen Length of the topic name
 * @param pParams Pointer to Publish Message parameters
 *
 * @return An IoT Error Type defining successful/failed publish
 */
IoT_Error_t tinyaiot_awsiot_mqtt_publish(AWS_IoT_Client *pClient, const char *pTopicName, uint16_t topicNameLen,
								 IoT_Publish_Message_Params *pParams);

/**
 * @brief Subscribe to an MQTT topic.
 *
 * Called to send a subscribe message to the broker requesting a subscription
 * to an MQTT topic.
 * @note Call is blocking.  The call returns after the receipt of the SUBACK control packet.
 *
 * @param pClient Reference to the IoT Client
 * @param pTopicName Topic Name to publish to
 * @param topicNameLen Length of the topic name
 * @param pApplicationHandler_t Reference to the handler function for this subscription
 * @param pApplicationHandlerData Data to be passed as argument to the application handler callback
 *
 * @return An IoT Error Type defining successful/failed subscription
 */
IoT_Error_t tinyaiot_awsiot_mqtt_subscribe(AWS_IoT_Client *pClient, const char *pTopicName, uint16_t topicNameLen,
								   QoS qos, pApplicationHandler_t pApplicationHandler, void *pApplicationHandlerData);

/**
 * @brief Subscribe to an MQTT topic.
 *
 * Called to resubscribe to the topics that the client has active subscriptions on.
 * Internally called when autoreconnect is enabled
 *
 * @note Call is blocking.  The call returns after the receipt of the SUBACK control packet.
 *
 * @param pClient Reference to the IoT Client
 *
 * @return An IoT Error Type defining successful/failed subscription
 */
IoT_Error_t tinyaiot_awsiot_mqtt_resubscribe(AWS_IoT_Client *pClient);

/**
 * @brief Unsubscribe to an MQTT topic.
 *
 * Called to send an unsubscribe message to the broker requesting removal of a subscription
 * to an MQTT topic.
 * @note Call is blocking.  The call returns after the receipt of the UNSUBACK control packet.
 *
 * @param pClient Reference to the IoT Client
 * @param pTopicName Topic Name to publish to
 * @param topicNameLen Length of the topic name
 *
 * @return An IoT Error Type defining successful/failed unsubscribe call
 */
IoT_Error_t tinyaiot_awsiot_mqtt_unsubscribe(AWS_IoT_Client *pClient, const char *pTopicFilter, uint16_t topicFilterLen);

/**
 * @brief Disconnect an MQTT Connection
 *
 * Called to send a disconnect message to the broker.
 *
 * @param pClient Reference to the IoT Client
 *
 * @return An IoT Error Type defining successful/failed send of the disconnect control packet.
 */
IoT_Error_t tinyaiot_awsiot_mqtt_disconnect(AWS_IoT_Client *pClient);

/**
 * @brief Yield to the MQTT client
 *
 * Called to yield the current thread to the underlying MQTT client.  This time is used by
 * the MQTT client to manage PING requests to monitor the health of the TCP connection as
 * well as periodically check the socket receive buffer for subscribe messages.  Yield()
 * must be called at a rate faster than the keepalive interval.  It must also be called
 * at a rate faster than the incoming message rate as this is the only way the client receives
 * processing time to manage incoming messages.
 *
 * @param pClient Reference to the IoT Client
 * @param timeout_ms Maximum number of milliseconds to pass thread execution to the client.
 *
 * @return An IoT Error Type defining successful/failed client processing.
 *         If this call results in an error it is likely the MQTT connection has dropped.
 *         iot_is_mqtt_connected can be called to confirm.
 */
IoT_Error_t tinyaiot_awsiot_mqtt_yield(AWS_IoT_Client *pClient, uint32_t timeout_ms);

/**
 * @brief MQTT Manual Re-Connection Function
 *
 * Called to establish an MQTT connection with the AWS IoT Service
 * using parameters from the last time a connection was attempted
 * Use after disconnect to start the reconnect process manually
 * Makes only one reconnect attempt Sets the client state to
 * pending reconnect in case of failure
 *
 * @param pClient Reference to the IoT Client
 *
 * @return An IoT Error Type defining successful/failed connection
 */
IoT_Error_t tinyaiot_awsiot_mqtt_attempt_reconnect(AWS_IoT_Client *pClient);

#ifdef __cplusplus
}
#endif

#endif

