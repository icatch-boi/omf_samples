#include "IAudioDevice.h"
#include "OmfMain.h"
#include "OmfDbg.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf::dev;
using namespace omf::api;
/////////////////////////////////////////////
using IMicrophone = omf::dev::IAudioDevice;
/////////////////////////////////////////////
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
	//
	OmfMain omf;
	//
	omf.ShowModules();
	omf.Debug(false);
	omf.LogConfig("all=false,err=true,note=true");
	//
	returnIfErrC(0,!GetMicrophoneInfo());
	//
	if(argc>1) {
		int vol = atoi(argv[1]);
		returnIfErrC(0, !SetMicrophoneVol(vol));
	}
	//
	//if(argc>1) {
	//	int mute = atoi(argv[1]);
	//	returnIfErrC(0,!SetMicrophoneMute(mute));
	//}

	return true;
}