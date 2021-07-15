#ifndef __TINYAIOT_SERVICES_DEF_H__
#define __TINYAIOT_SERVICES_DEF_H__

typedef enum _tinyaiotCloudServicesFunctionId_e {
    TINYAIOT_CLOUDSERVICES_FID_CONTROL,
    TINYAIOT_CLOUDSERVICES_FID_STORAGE,
    TINYAIOT_CLOUDSERVICES_FID_LIVE,
    TINYAIOT_CLOUDSERVICES_FID_REVIEW,
    TINYAIOT_CLOUDSERVICES_FID_MAX
} tinyaiotCloudServicesFunctionId_e;

typedef enum _tinyaiotCloudServicesCredentialId_e {
    TINYAIOT_CLOUDSERVICES_CID_AWSIOT,
    TINYAIOT_CLOUDSERVICES_CID_AWSIOT_CREDENTIALPROVIDER,
    TINYAIOT_CLOUDSERVICES_CID_ALIYUNIOT,
    TINYAIOT_CLOUDSERVICES_CID_MAX
} tinyaiotCloudServicesCredentialId_e;

typedef struct _tinyaiotCloudServicesCredentials_t{
	char *host;
	int port;
	tinyaiotCloudServicesCredentialId_e type;  /* NOTES: TINYAIOT_CLOUDSERVICES_CID_ALIYUNIOT Not Supported. */
	union {
		struct {
			char *caCert; /* String Buffer of the Certificate Authority(CA) certificate */
			char *clientCert; /* String Buffer of Device certificate signed by AWS IoT service */
			char *clientKey; /* String Buffer of Device private key */
			char *thingName;
			char *endpoint ; /* For TINYAIOT_CLOUDSERVICES_CID_AWSIOT_CREDENTIALPROVIDER, iot endpointType CredentialProvider*/
			char *roleAlias; /* For TINYAIOT_CLOUDSERVICES_CID_AWSIOT_CREDENTIALPROVIDER, iot endpointType CredentialProvider*/
		}awsiot; /* for type is TINYAIOT_CLOUDSERVICES_CID_AWSIOT or TINYAIOT_CLOUDSERVICES_CID_AWSIOT_* */
		struct {
			char *productSecret;
			char *productKey;
			char *deviceName;
			char *deviceSecret;
		}aliyuniot;  /* for type is TINYAIOT_CLOUDSERVICES_CID_ALIYUNIOT */
	}certificate;
} tinyaiotCloudServicesCredentials_t;

typedef enum _tinyaiotCloudServicesTypeId_e {
    TINYAIOT_CLOUDSERVICES_TID_AWS_IOT,
    TINYAIOT_CLOUDSERVICES_TID_AWS_S3,
    TINYAIOT_CLOUDSERVICES_TID_AWS_KVS_WEBRTC,
    TINYAIOT_CLOUDSERVICES_TID_AWS_KVS_STREAM,
    TINYAIOT_CLOUDSERVICES_TID_ALIYUN_OSS,
    TINYAIOT_CLOUDSERVICES_TID_MAX,
} tinyaiotCloudServicesTypeId_e;

typedef struct _tinyaiotCloudServicesInfo_t{
	tinyaiotCloudServicesTypeId_e type;  /* NOTES: TINYAIOT_CLOUDSERVICES_TID_ALIYUN_OSS Not Supported. */
	union {
		struct {
			char *region; /* aws region name, ex. ap-east-1 */
			char *thingName;
			char *endpoint ; /* iot endpointType CredentialProvider*/
			char *roleAlias; /* iot endpointType CredentialProvider*/
		}awsiot; /* for type is TINYAIOT_CLOUDSERVICES_TID_AWS_IOT */
		struct {
			char *region; /* aws region name, ex. ap-east-1 */
			char *bucketName; /*  */
			char *endpoint ; /*  */
			char *prefixPath;
		}aws3; /* for type is TINYAIOT_CLOUDSERVICES_TID_AWS_S3 */
		struct {
			char *region; /* aws region name, ex. ap-east-1 */
			char *name; /* streamname when type is AWS_KVS_STREAM, signalChannelName when type is AWS_KVS_WEBRTC */
		}awskvs; /* for type is TINYAIOT_CLOUDSERVICES_TID_AWS_KVS_* */
		struct {
			char *endpoint ; /*  */
			char *bucketName; /*  */
			char *prefixPath;
		}aliyunoss;  /* for type is TINYAIOT_CLOUDSERVICES_TID_ALIYUN_OSS */
	}services;
} tinyaiotCloudServicesInfo_t;

#endif
