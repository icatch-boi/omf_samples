#pragma once
#include "RtspServer.h"
#include "OmfRtspService.h"
#include <thread>
#include <mutex>
namespace omf{
namespace rtsp {
	class RtspService:public OmfRtspService{
	public:
		RtspService(const char*urlmap=0,const char*authmap=0);
		virtual ~RtspService();

	protected:
		bool Open() ;
		bool Close();

	public:
		virtual bool Start()override;
		virtual bool Stop()override;
	public:
		bool AddAuth(const char*);
		std::unique_ptr<std::thread> _thread;
		std::string _urlmap;
	private:
		UsageEnvironment* _env=0;
		RtspServer* _server=0;
		TaskScheduler* _scheduler=0;
		UserAuthenticationDatabase* _auth=0;

	protected:
		enum class State{
			null=0,
			ready=1,
			play=2,
		};
		State _sts =State::null;
		std::mutex _mtx;
	};
}
}