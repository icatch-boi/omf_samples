//
// Created by yb.leng on 2020/2/27.
//
#pragma once

//#include "_module.h"
#include "_base.h"
#include "./utility/AIModule.h"
#include "./utility/MessageCenter.h"
#include "./utility/PowerSource.h"
#include "./utility/Disk.h"
#include "./utility/DataInteractionSelection.h"
#include "./utility/VoicePrompt.h"
#include "./utility/RdtEventType.h"
#include "./utility/WifiStatus.h"
#include "./utility/DebugLogContrl.h"
#include "AttrSet.h"
typedef struct faceImagInfo_s{
	char* buf;
	int* sizeTbl;
	int szNum;
	int faceId;
	unsigned int bufSize;
}faceImagInfo_t;
/* allocate && free RTOS memeory */
void* RequestRtosMemory(unsigned int memSize);
bool ReleaseRtosMemory(void *ptr);

/* power-on */
uint32 PwrOnSrcGet();
bool PowerOnSoundPlay();

/* Read gpio level */
bool RealTimeReadPwrSrcGpioSts(uint32* status);
int PirEventStatusCheck();
int RingEventStatusCheck();
int SyncEventStatusCheck();
bool GpioInitPir();

/* wifi */
int WifiStatusGet();
bool WifiResetFlagGet(int* flag);
bool WifiVersionGet(std::string& val);
bool WifiMudulePushMsgEnable(bool opt);

/* setup */
bool IsSetupFlowEnd();
bool TriggerSetupFlow();

/* SD card */
bool IsSDCardMounted();
bool IsSDCardPlugIn();

/* misc */
bool PowerContrlFlagGet(uint32 opt, uint32* value);
bool PowerContrlFlagSet(uint32 opt, uint32 value);

/* first boot or reboot */
bool sysIsFirstBoot();
bool sysIsReboot();

/* ae stable */
int  AEStableGet();

/* message center */
bool MessageCenterFeatureStateGet(msg_center_sel_e sel, uint8* state);
bool MessageCenterFeatureSwitch(msg_center_sel_e sel, uint8 opt);
bool MessageCenterParamSet(msgCenterParam_t param);
bool MessageCenterParamGet(msgCenterParam_t* param);

/* factory setting */
bool FactorySetting(void);

/* record duration */
bool RecordDurationGet(uint32* value);
bool RecordDurationSet(uint32 value);

/* SD card size */
bool SDCardTotalSize(uint32* size);
bool SDCardInfo(void* info);
/* disk format */
bool DiskFormat(uint32 driver);

/* device version */
bool DeviceVersion(std::string& val);

/* device fota-upgrade */
bool DeviceFotaUpgradeCfgGet(uint32* opt);

/* reboot */
void RebootHostDevice();
/* OTA */
bool OTAFlagGet(uint8 *pFlag);
bool OTAFlagSet(uint8 flag);

/* pir */
bool PirStatusGet(uint32* val);
bool PirStatusSet(uint32 value);
bool PirRecordCfgGet(uint32* cfg);
bool PirRecordCfgSet(uint32 cfg);
bool PirMessageCfgGet(uint32* cfg);
bool PirMessageCfgSet(uint32 cfg);
bool PirSensitivityCfgGet(uint32* cfg);
bool PirSensitivityCfgSet(uint32 cfg);
bool PirSensitivitySpec(std::string& value);

/* push */
bool PushMessageViaWifiModule(uint32 msgType);
bool PushMsgViaWifiModuleTaskInit();
/* uuid */
bool SendUUIDToWifiModule(const char* uuid);
/* AI module */
bool AIModuleSwitch(AI_module_sel_e sel, uint8 opt);
bool AIModuleStateGet(AI_module_sel_e sel, uint8* state);

/* voice prompts */
void WavVoicePromptsPlay(voicePrompts_t opt,bool async);

/* power saving control */
bool PowerOffTimeSet(uint32 val);
bool PowerOffTimeGet(uint32* val);

/* USB */
bool IsUSBPlugIn();

/* battery */
bool BattChargingStatus(int* status);
bool BatteryCurrLvl(uint32* lvl);

/*device name*/
bool DevNameGet(std::string& devName);
bool DevNameSet(std::string devName);

/*time zone*/
bool TimeZoneGet(std::string& servZone, std::string& timeZone, std::string& dayLightSave);
bool TimeZoneSet(std::string servZone, std::string timeZone, std::string dayLightSave);

/* thumbnail */
bool ThumbnailFlagGet(std::string& value);

/* setup */
bool SetupFlagGet(int* flag);
bool SetupFlagSet(int flag);

/* wifi param */
bool WifiParamsGet(std::string& ssid, std::string& password);
bool WifiParamsSet(std::string ssid, std::string password);
/* UUID */
bool SystemUUIDGet(std::string& uuid);
bool SystemUUIDSet(std::string uuid);

/* face database */
bool FaceDatabaseAddImag(faceImagInfo_t info);
bool FaceDatabaseInfoGet(int *faceIDNum, int *maxFaceID);
bool FaceDatabaseDeleteFaceID(uint32 *pFIDArray, uint32 arraySize);
bool FaceDatabaseFaceIDNumGet(uint32 *pNum);
bool FaceDatabaseFaceIDListGet(uint32 *pList);

/* sqlite database */
bool SqliteDBFilePathGet(std::string &pathname, int fileID, int fileType);
bool SqliteDBFilePathGet(std::string &pathname, std::string data, int fileType);
bool SqliteDBFileSetFavorite(int fileID, const char* data, int &fileType);
bool SqliteDBFileDelete(int fileID, int fileType);
bool SqliteDBFileFilterGet(std::string &filer);
bool SqliteDBFileFilterSet(std::string filer);
bool SqliteDBFileListGet(std::string input, int mode, std::string &output);
bool SqliteDBNewFileCountGet(std::string input, int &output);
bool SqliteDBStorageInfoGet(std::string input, std::string &output);
bool SqliteDBRemoteTimeGet(std::string &output);
bool SqliteDBFileTimeRangeGet(std::string &output);

/* SDIO switch */
bool SDIOTransmitMethod(int *pMethod);
/* rdt event */
bool SendRdtEvent(uint32 sel, ...);
bool SendRdtEvent(AttrSet param);
/*  client number */
bool GlobalClientNumSet(uint32 clientNum);
bool GlobalClientNumGet(uint32 *pNum);

/* login result */
bool GlobalLoginResultSet(int result);
bool GlobalLoginResultGet(int *pResult);

/* login status */
bool GlobalLoginStatusSet(bool isNotOver);
bool GlobalLoginStatusGet(bool *pSatus);

/* get wakeup pattern flow status */
bool GlobalWakeupPtnStatusSet(bool isNotOver);
bool GlobalWakeupPtnStatusGet(bool *pSatus);

/* message push status */
bool GlobalMsgPushStatusSet(bool isNotOver);
bool GlobalMsgPushStatusGet(bool *pSatus);

/* file push status */
bool GlobalFilePushStatusSet(bool isNotOver);
bool GlobalFilePushStatusGet(bool *pSatus);

/* push message and file */
bool GlobalPush(const char *type, AttrSet param);

/*  set wakeup pattern to wifi module(MT7682) */
bool WakeUpPatternXmit(void *pBuf, uint32 size, uint32 checkSum, std::function<void(void*)> funcFree);
bool CheckWakeupInfo(uint32 checksum);

/* icatch_iot interface */
bool TinyAIotInit(void);
bool TinyAIotDeinit(void);
int TinyAIotMsgNotify(void* msgInfo);
int TinyAIotUpload(char* name, int type, void* info);
int TinyAIotDownloadFile(char* fileuri, char* filename);
int TinyAIotGetFileStatus(const char* name, void* status);

/* pass one day check */
bool RtcPassOneDayCheck();

/* CryptoCipherInfo */
bool CryptoCipherInfoGet(int *pResult, int *pLength, int *pBuf);

/* SDIO switch */
bool SDIOSwitchLinux(int *pBuf);
bool SDIOSwitchRtos();
bool SDIOXmitReceive(void *data, int size);

/*boot linux*/
bool LinuxBootStart();
/*package status*/
bool PackageStatusGet(uint8* status);
bool PackageStatusSet(uint8 status);
/*linux thread status*/
bool LinuxThreadIdle();

