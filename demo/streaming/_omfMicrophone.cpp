#include "IAudioDevice.h"
#include "OmfMain.h"
#include "OmfHelper.h"
#include "OmfDbg.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::dev;
using namespace omf::api;
/////////////////////////////////////////////
using IMicrophone = omf::dev::IAudioDevice;
/////////////////////////////////////////////
static int _vol = -1;
static int _mute = -1;
static int _info = 0;
/////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfMicrophone(...): \n"
		 "This demo shows how to control Microphone volume using IMicrophone interface.\n"
		 "> omfMicrophone -v 30\n"
		 "> omfMicrophone -m 1\n"
		},
		{"vol"		,'v'	, _vol		,"change the volume."},
		{"mute"		,'m'	, _mute		,"set or clear mute."},
		{"info"		,'s'	, _info		,"display the drive info."},
		{},
};
static IMicrophone* GetMicrophone() {
	static IMicrophone *_mic = 0;
	if(!_mic)
		_mic=IMicrophone::Microphone();
	return _mic;
}
bool SetMicrophoneVol (int vol){dbgTestPVL(vol);
	auto mic=GetMicrophone();
	returnIfErrCS(false,!mic,"current Microphone is null");
	returnIfErrC(false,!mic->IsSupportVolumeControl());
	returnIfErrC(false,!mic->Volume(vol));
	return true;
}
bool SetMicrophoneMute (int b_mute){dbgTestPVL(b_mute);
	auto mic=GetMicrophone();
	returnIfErrCS(false,!mic,"current Microphone is null");
	returnIfErrC(false,!mic->IsSupportMute());
	returnIfErrC(false,!mic->Mute(b_mute));
	return true;
}
bool GetMicrophoneInfo(){dbgTestPL();
	auto mic=GetMicrophone();
	returnIfErrCS(false,!mic,"current Microphone is null");
	dbgTestPVL(mic->Rate());
	dbgTestPVL(mic->Channels());
	dbgTestPVL(mic->Depth());///bit width per sample
	dbgTestPVL(mic->Width());///byte width per sample
	dbgTestPVL(mic->IsSupportVolumeControl());
	dbgTestPVL(mic->Volume());
	dbgTestPVL(mic->VolumeMax());
	dbgTestPVL(mic->VolumeMin());
	dbgTestPVL(mic->IsSupportMute());
	dbgTestPVL(mic->Mute());
	dbgTestPVL(mic->IsSupportAutoGainControl());
	dbgTestPVL(mic->AGC());
	return true;
}

//
int main(int argc,char* argv[]){
	dbgNotePSL("omfMicrophone(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	//
	if(_info>0)GetMicrophoneInfo();
	if(_vol>=0)SetMicrophoneVol(_vol);
	if(_mute>=0)SetMicrophoneMute(_mute);

	return true;
}