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
		 * send msg to target using default userID(set by Start(id))
		 * @param target [in]:the msg receiver id
		 * @param data [in]:the msg body
		 * @param size [in]:the msg body size
		 * @param flags [in]:the msg flags
		 * @return true/false
		 */
		virtual bool Send(int target, const void *data, int size, unsigned flags = 0)=0;
		/**
		 * send msg from sender to target
		 * @param sender [in]:the msg sender id
		 * @param target [in]:the msg receiver id
		 * @param data [in]:the msg body
		 * @param size [in]:the msg body size
		 * @param flags [in]:the msg flags
		 * @return true/false
		 */
		virtual bool Send(int sender,int target, const void *data, int size, unsigned flags = 0)=0;
	public:
		/**
		 * the receive callback
		 * @param sender [in]:the msg sender id
		 * @param target [in]:the msg receiver id
		 * @param data [in]:the msg body
		 * @param size [in]:the msg body size
		 * @param flags [in]:the msg flags
		 */
		using FuncProc = std::function<bool(const void *data, int size, int sender, int target, unsigned flags)>;
	public:///receive
		/**
		 * receive message with site ID.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Receive(const FuncProc &cb)=0;
		/**
		 * receive message with specified ID.
		 * @param target [in]:target ID.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Receive(int target, const FuncProc &cb)=0;
		/**
		 * receive message specified in the ID list.
		 * @param targets [in]: the target ID list.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Receive(std::initializer_list<int> targets, const FuncProc &cb)=0;
	public:///register,
		/**
		 * register the callback to receive message with the site ID automatically.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Register(const FuncProc &cb)=0;
		/**
		 * register the callback to receive message with the specified ID automatically.
		 * @param id [in]:the target ID.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Register(int id, const FuncProc &cb)=0;
		/**
		 * register the callback to receive message specified in the ID list.
		 * @param ids [in]:the target ID list.
		 * @param cb [in]:the receive callback
		 * @return true/false
		 */
		virtual bool Register(std::initializer_list<int> ids, const FuncProc &cb)=0;
	public:
		static OmfMsgSite* Globle();
	};
}

