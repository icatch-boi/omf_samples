//
// Created by jg.wang on 2020/2/13.
//

#pragma once

#include <functional>
namespace omf {
	class OmfMsgSite {
	protected:
		virtual ~OmfMsgSite(){}
	public:
		/**
		 * Get the Site ID.
		 * @return site ID.
		 */
		virtual int GetSiteID()const = 0;
		/**
		 * Check the Site is started and working properly.
		 * @return true/false
		 */
		virtual operator bool()const = 0;
	public:///send
		/**
		 * send msg using siteID
		 * @param msgID [in]:the msg id
		 * @param data [in]:the msg body
		 * @param size [in]:the msg body size
		 * @param flags [in]:the msg flags
		 * @return true/false
		 */
		virtual bool Send(int msgID, const void *data, int size, unsigned flags = 0)=0;
		/**
		 * send msg use sourceID
		 * @param sourceID [in]:the msg source id
		 * @param msgID [in]:the msg id
		 * @param data [in]:the msg body
		 * @param size [in]:the msg body size
		 * @param flags [in]:the msg flags
		 * @return true/false
		 */
		virtual bool Send(int sourceID,int msgID, const void *data, int size, unsigned flags = 0)=0;
	public:
		/**
		 * the receive callback
		 * @param sender [in]:the msg id
		 * @param target [in]:the msg source id
		 * @param data [in]:the msg body
		 * @param size [in]:the msg body size
		 * @param flags [in]:the msg flags
		 */
		using FuncProc = std::function<bool(const void *data, int size, int sourceID, int msgID, unsigned flags)>;
	public:///receive
		/**
		 * receive message with site ID.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Receive(const FuncProc &cb)=0;
		/**
		 * receive message with msg ID.
		 * @param msgID [in]:msg ID.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Receive(int msgID, const FuncProc &cb)=0;
		/**
		 * receive message specified in the msgID list.
		 * @param msgIDs [in]: the msg ID list.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Receive(std::initializer_list<int> msgIDs, const FuncProc &cb)=0;
	public:///register,
		/**
		 * register the callback to receive message with the site ID automatically.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Register(const FuncProc &cb)=0;
		/**
		 * register the callback to receive message with the specified msg ID automatically.
		 * @param msgID [in]:the msg ID.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Register(int msgID, const FuncProc &cb)=0;
		/**
		 * register the callback to receive message specified in the msg ID list.
		 * @param ids [in]:the msg ID list.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Register(std::initializer_list<int> msgIDs, const FuncProc &cb)=0;
		/**
		 * unregister the Site ID process function.
		 * @return true/fasle
		 */
		virtual bool UnRegister()=0;
		/**
		 * unregisger the msgID process function.
		 * @param msgID[in]
		 * @return true/false
		 */
		virtual bool UnRegister(int msgID)=0;
		/**
		 * unregisger the msgIDs process function.
		 * @param msgIDs
		 * @return true/false
		 */
		virtual bool UnRegister(std::initializer_list<int> msgIDs)=0;
	public:
		static OmfMsgSite* Globle();
	};
}

