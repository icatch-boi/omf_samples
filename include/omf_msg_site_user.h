//
// Created by jg.wang on 2020/3/26.
//
#pragma once
#ifndef STREAM_OMF_MSG_USER_H
#define STREAM_OMF_MSG_USER_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

/**
 * send msg with the name "AAA"
 * @param name [in]:msg name
 * @param data [in]:msg body
 * @param size [in]:msg size
 * @return true/false
 */
EXTERNC int omfMsgSiteUserSend(const char* name, const void *data, int size);

/**
 * the receive callback
 * @param hd [in]: the input private data handle
 * @param data [in]:msg body
 * @param size [in]:msg size
 */
typedef int(*OmfMsgSiteUserCbProc)(void* priv_hd, const void *data, int size);
/**
 * receive message with cb.
 * @param name [in]: msg name.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC int omfMsgSiteUserReceiveCb(const char* name, OmfMsgSiteUserCbProc cb, void*priv_hd);

/**
 * receive message with cb.
 * @param name [in]: msg name.
 * @param data [in]:msg buff
 * @param max [in]:buff max size
 * @return msg body size.
 */
EXTERNC int omfMsgSiteUserReceive(const char* name, void *data, int max);

/**
 * register the receiver to msg site.
 * @param name [in]: msg name.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC int omfMsgSiteUserRegister(const char* name, OmfMsgSiteUserCbProc cb, void*priv_hd);
/**
 * unregister the receiver with name.
 * @param name [in]: msg name.
 * @return true/fasle
 */
EXTERNC int omfMsgSiteUserUnRegister(const char* name);
#endif //STREAM_OMF_MSG_SITE_H
