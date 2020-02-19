//
// Created by jg.wang on 2020/2/13.
//

#pragma once

#include <functional>
namespace omf {
	class IMsgSite {
	public:
		virtual ~IMsgSite();
	public:
		/**
		 * start the Message site with the site ID.
		 * @param id [in]:set the site ID.
		 * @return true/false
		 */
		virtual bool Start(int id)=0;
		/**
		 * stop the Message site flow.
		 * @return true/fasle
		 */
		virtual bool Stop()=0;
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
		static IMsgSite& Globle();
	};
}
#if 0
class IMsgSite{
public:
	virtual bool Start(int id)=0;
	virtual bool Stop()=0;
public:///send
	virtual bool Send(int target,const void*data,int size,unsigned flags=0)=0;
	virtual bool Send(int sender,int target,const void*data,int size,unsigned flags=0)=0;
public:
	using FuncProc = std::function<bool(void*data,int size,int sender,int target,unsigned flags)>;
public:///receive
	virtual int Receive(const FuncProc&)=0;
	virtual int Receive(int target,const FuncProc&)=0;
	virtual int Receive(std::initializer_list<int>ids,const FuncProc&)=0;
//public:
//	virtual int Receive(void*data,int max,int& sender,unsigned& flags)=0;
//	virtual int Receive(int target,void*data,int max,int& sender,unsigned& flags)=0;
//	virtual int Receive(std::initializer_list<int>ids,void*data,int max,int& sender,int& target,unsigned& flags)=0;
public:
	virtual bool Register(const FuncProc&)=0;
	virtual bool Register(int id,const FuncProc&)=0;
	virtual bool Register(std::initializer_list<int>ids,const FuncProc&)=0;
public:
	virtual bool Send(AttrSet&)=0;
	virtual bool Receive(AttrSet&)=0;
public:
	virtual bool Register(const FuncProcX&)=0;
	virtual bool Register(int id,const FuncProcX&)=0;
	virtual bool Register(std::initializer_list<int>ids,const FuncProcX&)=0;

public:
			using FuncProcX = std::function<bool(AttrSet&)>;
};
class IMsgSite{
public:
	virtual bool Start(int id)=0;
	virtual bool Stop()=0;
public:///send
	/**
	 * send a msg to target.
	 * @param data :the msg context
	 * @param size :data size
	 * @param target :the msg receiver
	 * @param flags :the flags,default is 0
	 * @param sender :the sender,default is ID of the MsgSite.
	 * @return true/false
	 */
	virtual bool Send(void*data,int size,int target,unsigned flags=0,int sender=0)=0;
	/**
	 * send msg with ap
	 * @param ap :context,sender,target,flags
	 * @return true/false
	 */
	virtual bool Send(AttrSet&ap)=0;
public:///receive
	virtual int Receive(void*data,int max,int& sender,unsigned& flags,int target=0)=0;
	virtual int Receive(std::initializer_list<int>ids,void*data,int max,int& sender,unsigned& flags,int& target)=0;
	virtual bool Receive(AttrSet&ap)=0;
public:
	using FuncProc0 = std::function<bool(AttrSet&)>;
	using FuncProc1 = std::function<bool(void*data,int size,int sender,int target,unsigned flags)>;
public:
	virtual bool Register(const FuncProc0&)=0;
	virtual bool Register(const FuncProc1&)=0;
public:
	virtual bool Register(int id,const FuncProc0&)=0;
	virtual bool Register(int id,const FuncProc1&)=0;
	virtual bool Register(std::initializer_list<int>ids,const FuncProc0&)=0;
	virtual bool Register(std::initializer_list<int>ids,const FuncProc1&)=0;
};
#endif
