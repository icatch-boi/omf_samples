#include "omf_api.h"
#include "IAudioDevice.h"
#include "OmfDbg.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf::dev;
/////////////////////////////////////////////
using ISpeaker = omf::dev::IAudioDevice;
/////////////////////////////////////////////
static ISpeaker* GetSpeaker() {
	static ISpeaker *_speaker = 0;
	if(!_speaker)
		_speaker=ISpeaker::Speaker();
	return _speaker;
}
bool SetSpeakerVol (int vol){dbgTestPVL(vol);
	auto speaker=GetSpeaker();
	returnIfErrCS(false,!speaker,"current Speaker is null");
	returnIfErrC(false,!speaker->IsSupportVolumeControl());
	returnIfErrC(false,!speaker->Volume(vol));
	return true;
}
bool SetSpeakerMute (int b_mute){dbgTestPVL(b_mute);
	auto speaker=GetSpeaker();
	returnIfErrCS(false,!speaker,"current Speaker is null");
	returnIfErrC(false,!speaker->IsSupportMute());
	returnIfErrC(false,!speaker->Mute(b_mute));
	return true;
}
bool GetSpeakerInfo(){dbgTestPL();
	auto speaker=GetSpeaker();
	returnIfErrCS(false,!speaker,"current Speaker is null");
	dbgTestPVL(speaker->Rate());
	dbgTestPVL(speaker->Channels());
	dbgTestPVL(speaker->Depth());///bit width per sample
	dbgTestPVL(speaker->Width());///byte width per sample
	dbgTestPVL(speaker->IsSupportVolumeControl());
	dbgTestPVL(speaker->Volume());
	dbgTestPVL(speaker->VolumeMax());
	dbgTestPVL(speaker->VolumeMin());
	dbgTestPVL(speaker->IsSupportMute());
	dbgTestPVL(speaker->Mute());
	dbgTestPVL(speaker->IsSupportAutoGainControl());
	dbgTestPVL(speaker->AGC());
	return true;
}

//
int main(int argc,char* argv[]){
	//
	//initialize the omf system
	returnIfErrC(false,!omfInit(0));
	//
	returnIfErrC(0,!GetSpeakerInfo());
	//
	if(argc>1) {
		int vol = atoi(argv[1]);
		returnIfErrC(0, !SetSpeakerVol(vol));
	}
	//
	//if(argc>1) {
	//	int mute = atoi(argv[1]);
	//	returnIfErrC(0,!SetSpeakerMute(mute));
	//}
	//uninitialize the omf system
	returnIfErrC(false,!omfUninit(0));
	return true;
}