//
// Created by jg.wang on 2020/2/14.
//
#pragma once
#ifndef STREAM_OMF_MSG_SITE_H
#define STREAM_OMF_MSG_SITE_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

/**
 * get the msg site with ID 0.
 * @return the msg site handle.
 */
EXTERNC void* omfMsgSite0Get();
/**
 * Get the Site ID.
 * @param site [in]:the site handle.
 * @return site ID.
 */
EXTERNC int omfMsgSiteGetID(void*site);
/**
 * Check the Site is started and working properly.
 * @param site [in]:the site handle.
 * @return true/false
 */
EXTERNC int omfMsgSiteIsWorking(void*site);
///send
/**
 * send msg to target using default userID(set by Start(id))
 * @param site [in]:the site handle.
 * @param target [in]:the msg receiver id
 * @param data [in]:the msg body
 * @param size [in]:the msg body size
 * @param flags [in]:the msg flags
 * @return true/false
 */
EXTERNC int omfMsgSiteSend(void*site, int target, const void *data, int size, unsigned flags);
/**
 * send msg from sender to target
 * @param site [in]:the site handle.
 * @param sender [in]:the msg sender id
 * @param target [in]:the msg receiver id
 * @param data [in]:the msg body
 * @param size [in]:the msg body size
 * @param flags [in]:the msg flags
 * @return true/false
 */
EXTERNC int omfMsgSiteSend1(void*site, int sender,int target, const void *data, int size, unsigned flags);
/**
 * the receive callback
 * @param hd [in]: the input private data handle
 * @param sender [in]:the msg sender id
 * @param target [in]:the msg receiver id
 * @param data [in]:the msg body
 * @param size [in]:the msg body size
 * @param flags [in]:the msg flags
 */
typedef int(*OmfMsgSiteFuncProc)(void* hd, const void *data, int size, int sender, int target, unsigned flags);
/**
 * receive message with site ID.
 * @param site [in]:the site handle.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC bool omfMsgSiteReceive(void*site, OmfMsgSiteFuncProc cb, void*hd);
/**
 * receive message with specified ID.
 * @param site [in]:the site handle.
 * @param target [in]:target ID.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC bool omfMsgSiteReceive1(void*site, int target, OmfMsgSiteFuncProc cb, void*hd);
/**
 * receive message specified in the ID list.
 * @param site [in]:the site handle.
 * @param targets [in]: the target ID list. end of 0.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC bool omfMsgSiteReceive2(void*site, int* targets, OmfMsgSiteFuncProc cb, void*hd);
///register,
/**
 * register the callback to receive message with the site ID automatically.
 * @param site [in]:the site handle.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC bool omfMsgSiteRegister(void*site, OmfMsgSiteFuncProc cb, void*hd);
/**
 * register the callback to receive message with the specified ID automatically.
 * @param site [in]:the site handle.
 * @param id [in]:the target ID.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC bool omfMsgSiteRegister1(void*site, int id, OmfMsgSiteFuncProc cb, void*hd);
/**
 * register the callback to receive message specified in the ID list.
 * @param site [in]:the site handle.
 * @param targets [in]:the target ID list,end of 0.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC bool omfMsgSiteRegister2(void*site, int* targets, OmfMsgSiteFuncProc cb, void*hd);


#endif //STREAM_OMF_MSG_SITE_H
