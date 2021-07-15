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
using ISpeaker = omf::dev::IAudioDevice;
/////////////////////////////////////////////
static int _vol = -1;
static int _mute = -1;
static int _info = 0;
/////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfSpeaker(...): \n"
		 "This demo shows how to control speaker volume using ISpeaker interface.\n"
		 "> omfSpeaker -v 30\n"
		 "> omfSpeaker -m 1\n"
		},
		{"vol"	,'v', _vol	,"change the volume."},
		{"mute"	,'m', _mute	,"set or clear mute."},
		{"info"	,'s', _info	,"display the drive info."},
		{},
};

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
	dbgNotePSL("omfSpeaker(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///
	if(_info>0)GetSpeakerInfo();
	if(_vol>=0)SetSpeakerVol(_vol);
	if(_mute>=0)SetSpeakerMute(_mute);

	return true;
}