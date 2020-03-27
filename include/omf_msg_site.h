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
 * get the default msg site.
 * @return the msg site handle.
 */
EXTERNC void* omfMsgSite0Get();
/**
 * Get the Site ID.
 * @param site [in]:the site handle.
 * @return site ID.
 */
EXTERNC unsigned omfMsgSiteGetID(void*site);
/**
 * Check the Site is started and working properly.
 * @param site [in]:the site handle.
 * @return true/false
 */
EXTERNC int omfMsgSiteIsWorking(void*site);
///send
/**
 * send msg with Site ID
 * @param site [in]:the site handle.
 * @param receiver [in]:the msg id
 * @param data [in]:the msg body
 * @param size [in]:the msg body size
 * @param flags [in]:the msg flags
 * @return true/false
 */
EXTERNC int omfMsgSiteSend(void*site, unsigned receiver, const void *data, int size, unsigned flags);
/**
 * send msg with SourceID
 * @param site [in]:the site handle.
 * @param sender [in]:the msg source id
 * @param receiver [in]:the msg id
 * @param data [in]:the msg body
 * @param size [in]:the msg body size
 * @param flags [in]:the msg flags
 * @return true/false
 */
EXTERNC int omfMsgSiteSend1(void*site, unsigned sender,unsigned receiver, const void *data, int size, unsigned flags);
/**
 * the receive callback
 * @param hd [in]: the input private data handle
 * @param sender [in]:the msg sender id
 * @param target [in]:the msg receiver id
 * @param data [in]:the msg body
 * @param size [in]:the msg body size
 * @param flags [in]:the msg flags
 */
typedef int(*OmfMsgSiteFuncProc)(void* hd, const void *data, int size, unsigned sender, unsigned receiver, unsigned flags);
/**
 * receive message with site ID.
 * @param site [in]:the site handle.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC int omfMsgSiteReceive(void*site, OmfMsgSiteFuncProc cb, void*hd);
/**
 * receive message with specified receiver.
 * @param site [in]:the site handle.
 * @param receiver [in]: msg ID.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC int omfMsgSiteReceive1(void*site, unsigned receiver, OmfMsgSiteFuncProc cb, void*hd);
/**
 * receive message specified in the receiver list.
 * @param site [in]:the site handle.
 * @param receivers [in]: the receiver list. end of 0.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC int omfMsgSiteReceive2(void*site, unsigned* receivers, OmfMsgSiteFuncProc cb, void*hd);
///register,
/**
 * register the callback to receive message with the site ID automatically.
 * @param site [in]:the site handle.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC int omfMsgSiteRegister(void*site, OmfMsgSiteFuncProc cb, void*hd);
/**
 * register the callback to receive message with the specified receiver automatically.
 * @param site [in]:the site handle.
 * @param receiver [in]:the msg ID.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC int omfMsgSiteRegister1(void*site, unsigned receiver, OmfMsgSiteFuncProc cb, void*hd);
/**
 * register the callback to receive message specified in the receiver list.
 * @param site [in]:the site handle.
 * @param receivers [in]:the receiver list,end of 0.
 * @param cb [in]:the receive callback
 * @param hd [in]:the private data handle, send to OmfMsgSiteFuncProc.
 * @return true/false
 */
EXTERNC int omfMsgSiteRegister2(void*site, unsigned* receivers, OmfMsgSiteFuncProc cb, void*hd);

/**
 * unregister the Site ID process function.
 * @param site [in]:the site handle.
 * @return true/fasle
 */
EXTERNC int omfMsgSiteUnRegister(void*site);
/**
 * unregisger the receiver process function.
 * @param site [in]:the site handle.
 * @param receiver [in]:the msg ID.
 * @return true/false
 */
EXTERNC int omfMsgSiteUnRegister1(void*site,unsigned receiver);
/**
 * unregisger the receivers process function.
 * @param site [in]:the site handle.
 * @param receivers [in]:the receiver list,end of 0.
 * @return true/false
 */
EXTERNC int omfMsgSiteUnRegister2(void*site,unsigned* receivers);

#endif //STREAM_OMF_MSG_SITE_H
