//
// Created by jg.wang on 2020/2/13.
//

#pragma once

#include <functional>
namespace omf {
	class OmfMsgSiteUser {
	public:///send
		/**
		 * send msg.
		 * @param name [in]:msg name
		 * @param data [in]: msg body
		 * @param size [in]: msg size
		 * @return true/false
		 */
		static bool Send(const char* name, const void *data, int size);
	public:
		/**
		 * the receive callback
		 * @param data [in]:msg body
		 * @param size [in]:msg size
		 */
		using FuncProc = std::function<bool(const void *data, int size)>;
	public:///receive
		/**
		 * receive message
		 * @param name [in]: msg name.
		 * @param cb [in]: receive callback
		 * @return true/false
		 */
		static bool Receive(const char* name, const FuncProc &cb);
	public:///register
		/**
		 * register the receive callback to msg site.
		 * @param name [in]: msg name.
		 * @param cb [in]: receive callback
		 * @return true/false
		 */
		static bool Register(const char* name, const FuncProc &cb);
		/**
		 * unregister the msg receiver.
		 * @param name [in]: msg name.
		 * @return true/fasle
		 */
		static bool UnRegister(const char* name);
	};
}

