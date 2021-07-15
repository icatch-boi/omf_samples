#include "RtspSmssPcm.h"
#include "_base.h"
using namespace NAMESPACE_ICATCH_PLUGIN::rtsp;

DEFINE_REGISTER(RtspSmssPcm) {
	TClass<UsageEnvironment*,PacketRing*>::Register<RtspSmssPcm>(
		 "rtsp-smss-pcm-live555", {
		})->Init(LIBRARY, VERSION(0), "iCatchtek", "jg.wang");
}
RtspSmssPcm::RtspSmssPcm(UsageEnvironment *env,PacketRing* ring)
	:RtspSmss(env,ring)
{
	_type="audio";
}
bool RtspSmssPcm::Parse(){
	IMedia* media = _ring->Media();
	returnIfErrC(false, !media);
	returnIfErrC(false, !media->Get("rate", _rate));
	returnIfErrC(false, !media->Get("channel", _channels));
	returnIfErrC(false, !media->Get("width", _width));
	returnIfErrC(false, !media->Get("depth", _depth));
	returnIfErrC(false, !media->Get("little", _little));
	returnIfErrC(false, !media->Get("signed", _signed));
	dbgLogPVL(_rate);
	dbgLogPVL(_channels);
	dbgLogPVL(_width);
	dbgLogPVL(_depth);
	dbgLogPVL(_little);
	dbgLogPVL(_signed);

	switch (_depth) {
		case 16:
			_codec = "L16";
			if(_little)
				_copy = (CbMemcpy)::Swap16Cpy;
			if (_rate == 44100 && _channels == 2)
				_payloadtype = 10; // a static RTP payload type
			else if (_rate == 44100 && _channels == 1)
				_payloadtype = 11; // a static RTP payload type
			break;
		case 20:
			_codec = "L20";
			break;
		case 24:
			_codec = "L24";
			break;
		default: // depth == 8 (we assume that depth == 4 is only for WA_IMA_ADPCM)
			_codec = "L8";
			break;
	}
	return true;
}


