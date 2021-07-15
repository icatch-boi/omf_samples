#include "RtpSource.h"
#include "OmfDbg.h"
///////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntryPink(s)
/////////////////////////////////////////////
using namespace rtsp;
RtpSource::RtpSource(UsageEnvironment & env, ISource* reader)
	: FramedSource(env)
	, _reader(reader)
	, _us(1000)
	, _1st(true)
{
	dbgTestPSL((void*)_reader);
	_reader->ChangeUp(ISource::State::ready);
}
RtpSource::~RtpSource(){
	//delete _reader;
	_reader->ChangeDown(ISource::State::ready);
}
void RtpSource::doGetNextFrame(){dbgTestPL();
	//nextTask()=envir().taskScheduler().scheduleDelayedTask(0,(TaskFunc*)Process,this);
	_reader->ChangeUp(ISource::State::play);
}
bool RtpSource::EOS(){
	handleClosure();
	return false;
}

std::shared_ptr<Packet> RtpSource::GeneratePacket(){
	std::shared_ptr<Packet> frm;
	if(!_reader->PullFrame(frm,false)) {//dbgTestPSL((void*)_reader);
		return nullptr;
	}
	if(!frm){
		return nullptr;
	}
	///
	if(_1st) {
		while (!frm->iskeyframe) {
			if (!_reader->PullFrame(frm, false)) {//dbgTestPSL((void*)_reader);
				return nullptr;
			}
			if (!frm) {
				return nullptr;
			}
		}
		///
		_1st = false;
	}
	//dbgTestPSL((void*)_reader);
	return frm;
}

int RtpSource::DelayTimeUs() const{
	return _us;
}