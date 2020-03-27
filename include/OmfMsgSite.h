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
		virtual unsigned GetSiteID()const = 0;
		/**
		 * Check the Site is started and working properly.
		 * @return true/false
		 */
		virtual operator bool()const = 0;
	public:///send
		/**
		 * send msg using siteID
		 * @param target [in]:the msg id
		 * @param data [in]:the msg body
		 * @param size [in]:the msg body size
		 * @param flags [in]:the msg flags
		 * @return true/false
		 */
		virtual bool Send(unsigned target, const void *data, int size, unsigned flags = 0)=0;
		/**
		 * send msg use sender
		 * @param sender [in]:the msg source id
		 * @param target [in]:the msg id
		 * @param data [in]:the msg body
		 * @param size [in]:the msg body size
		 * @param flags [in]:the msg flags
		 * @return true/false
		 */
		virtual bool Send(unsigned sender,unsigned target, const void *data, int size, unsigned flags = 0)=0;
	public:
		/**
		 * the receive callback
		 * @param sender [in]:the msg id
		 * @param target [in]:the msg source id
		 * @param data [in]:the msg body
		 * @param size [in]:the msg body size
		 * @param flags [in]:the msg flags
		 */
		using FuncProc = std::function<bool(const void *data, int size, unsigned sender, unsigned target, unsigned flags)>;
	public:///receive
		/**
		 * receive message with site ID.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Receive(const FuncProc &cb)=0;
		/**
		 * receive message with msg ID.
		 * @param target [in]:msg ID.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Receive(unsigned target, const FuncProc &cb)=0;
		/**
		 * receive message specified in the target list.
		 * @param target [in]: the msg ID list.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Receive(std::initializer_list<unsigned> target, const FuncProc &cb)=0;
		/**
		 * receive message specified in the target list.
		 * @param target [in]: the msg ID list,endof 0;
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Receive(unsigned* target, const FuncProc &cb)=0;
	public:///register,
		/**
		 * register the callback to receive message with the site ID automatically.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Register(const FuncProc &cb)=0;
		/**
		 * register the callback to receive message with the specified msg ID automatically.
		 * @param target [in]:the msg ID.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Register(unsigned target, const FuncProc &cb)=0;
		/**
		 * register the callback to receive message specified in the msg ID list.
		 * @param target [in]:the msg ID list.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Register(std::initializer_list<unsigned> target, const FuncProc &cb)=0;
		/**
		 * register the callback to receive message specified in the msg ID list.
		 * @param target [in]:the msg ID list,endof 0;
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Register(unsigned* target, const FuncProc &cb)=0;
		/**
		 * unregister the Site ID process function.
		 * @return true/fasle
		 */
		virtual bool UnRegister()=0;
		/**
		 * unregisger the target process function.
		 * @param target[in]
		 * @return true/false
		 */
		virtual bool UnRegister(unsigned target)=0;
		/**
		 * unregisger the target process function.
		 * @param target
		 * @return true/false
		 */
		virtual bool UnRegister(std::initializer_list<unsigned> target)=0;
		/**
		 * unregisger the target process function.
		 * @param target [in]:the msg ID list,endof 0;
		 * @return true/false
		 */
		virtual bool UnRegister(unsigned* target)=0;
	public:
		static OmfMsgSite* Globle();
	};
}

