#ifndef __TINYAIOT_DATA_DEF_H__
#define __TINYAIOT_DATA_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#define TINYAIOT_MAX_DEVICE_ID_LENGTH      (48)
#define TINYAIOT_MAX_DEVICE_UID_LENGTH     (48)
#define TINYAIOT_MAX_DEVICE_OWNERID_LENGTH (48)
#define TINYAIOT_MAX_DEVICE_NAME_LENGTH    (128)
#define TINYAIOT_MAX_DEVICE_SECRET_LENGTH  (256)
#define TINYAIOT_MAX_DEVICE_TIME_LENGTH    (23+9)

typedef enum _tinyaiotTransTypeId_e {
    TINYAIOT_TRANS_TYPE_ID_HTTP   = 0,
    TINYAIOT_TRANS_TYPE_ID_SOCKET = 1,
    TINYAIOT_TRANS_TYPE_ID_MQTT   = 2,
    TINYAIOT_TRANS_TYPE_ID_MAX,
} tinyaiotTransTypeId_e;

typedef enum _tinyaiotDevTypeId_e {
    TINYAIOT_DEV_TYPE_CAMERA   = 1,
    TINYAIOT_DEV_TYPE_IPCAMERA = 2,
    TINYAIOT_DEV_TYPE_DOORBELL = 3,
    TINYAIOT_DEV_TYPE_MAX,
} tinyaiotDevTypeId_e;

typedef enum _tinyaiotFileTypeId_e {
    TINYAIOT_FILE_TYPE_MOV  = 1,    /* video file(MOV) */
    TINYAIOT_FILE_TYPE_JPEG = 2,    /* picture file(JPG) */
    TINYAIOT_FILE_TYPE_MP3  = 3,    /* audio file(MP3) */
    TINYAIOT_FILE_TYPE_TEXT = 4,    /* text file */
    TINYAIOT_FILE_TYPE_IMG  = 5,    /* image file */
    TINYAIOT_FILE_TYPE_PIR  = 0x11, /* PIR file,include thumbnail file */
    TINYAIOT_FILE_TYPE_FACE = 0x12, /* face jpeg file */
    TINYAIOT_FILE_TYPE_MSG  = 0x13, /* message jpeg file */
    TINYAIOT_FILE_TYPE_MAX,
} tinyaiotFileTypeId_e;

typedef enum _tinyaiotParamId_e {
    TINYAIOT_PARAM_ID_DEV_SERVER_ADDR  = 1,    /* data value is a string */
    TINYAIOT_PARAM_ID_FILE_SERVER_ADDR = 2,    /* data value is a string */
    TINYAIOT_PARAM_ID_MSG_SERVER_ADDR  = 3,    /* data value is a string */
    TINYAIOT_PARAM_ID_UUID_SERVER_ADDR = 4,    /* data value is a string */
    TINYAIOT_PARAM_ID_OAUTH2_AUTHCODE  = 0x10, /* data value is a string */
    TINYAIOT_PARAM_ID_DEVICE_CHARACTER = 0x11, /* data value is a structure of tinyaiotCharacter_t */
    TINYAIOT_PARAM_ID_OAUTH2_CLIENTS   = 0x12, /* data value is a structure of tinyaiotOauth2Client_t */
    TINYAIOT_PARAM_ID_MAX,
} tinyaiotParamId_e;

typedef struct _tinyaiotCharacter_t {
    char *mac;
    char *pid;
    char *vid;
} tinyaiotCharacter_t;

typedef struct _tinyaiotOauth2Client_t {
    char *id;
    char *secret;
    char *redirect_uri;
} tinyaiotOauth2Client_t;

typedef struct _tinyaiotDevInfo_t {
    char *uid;
    char *id;
    char *name;
    char *hwversionid;
    char *versionid;
    char *password;
    char *ownerid;
    char *time;
    tinyaiotDevTypeId_e type;
} tinyaiotDevInfo_t;

typedef struct _tinyaiotDevInfoArray_t {
    char uid[TINYAIOT_MAX_DEVICE_UID_LENGTH];
    char id[TINYAIOT_MAX_DEVICE_ID_LENGTH];
    char name[TINYAIOT_MAX_DEVICE_NAME_LENGTH];
    char password[TINYAIOT_MAX_DEVICE_SECRET_LENGTH];
    char ownerid[TINYAIOT_MAX_DEVICE_OWNERID_LENGTH];
    char time[TINYAIOT_MAX_DEVICE_TIME_LENGTH]; /* 2019-01-01T00.00.00.000 */
    tinyaiotDevTypeId_e type;
} tinyaiotSaveDevInfoArray_t;

typedef struct _tinyaiotDevStatus_t {
    int batterylevel;  /* 0~100 */
    int switchlevel;   /* 0~100 */
    int tfcapacity;
    int temperature;
    int humidity;
    int atmos;
    char * onlineexpires;       /* device offline time, iso8601's string */
    char * extensions;
} tinyaiotDevStatus_t;

typedef struct _tinyaiotFileInfo_t {
    char *data;
    int   size;
    int   chunked;  /* chunked mode:
                       0: non chunked mode;
                       1: chunk mode, data to be continued;
                       2: chunk mode, data complete.
                    */
    char *iso_time;
} tinyaiotFileInfo_t;

typedef struct _tinyaiotFileAttrInfo_t {
    char *time;
    int   type;
    char *path;
    char *name;
    char *info;
    int   size;
    int   duration;
    int   monitor;
} tinyaiotFileAttrInfo_t;

typedef struct _tinyaiotFileStatus_t {
    int type;       /* 1:upload, 2:download */
    int completed;  /* bytes */
    int total;      /* bytes */
} tinyaiotFileStatus_t;

typedef struct _tinyaiotMessage_t {
    int  msgType;
    char *time;
    int  msgId;
    char *msgParam;
    char *attachment;
    int  errCode;
    char* extraParam;  /* json format string */
    //void* data;
	int format; /* 4: tutk push compatible, others:standard tinyaiot push;  */
} tinyaiotMessage_t;

typedef int (*tinyaiotDevInfoUpdateCallbackFunction_t)(const tinyaiotDevInfo_t *);
typedef int (*tinyaiotDevInfoGetCallbackFunction_t)(tinyaiotSaveDevInfoArray_t *);

#ifdef __cplusplus
}
#endif

#endif

