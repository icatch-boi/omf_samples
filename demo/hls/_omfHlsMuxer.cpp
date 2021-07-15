#include <thread>
#include <list>
#include <mutex>
#include <sys/statfs.h>
#include "_hash.h"
#include "_chrono_base.h"
#include "OmfMain.h"
#include "OmfObject.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "OmfHelper.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::api;
using namespace omf::chrono;
////////////////////////////////////////////
static const char* _fname="REC_%04u-%02u-%02u_%02u-%02u-%02u.mp4";
static bool _fnameWithRtc = true;
///
static int _seamlessSeconds=0;
static int _recordSeconds=10;
static bool _powerFailProtection=true;
/////////////////////////////////////////////////////////
/// the codec is required, such as aac,pcm
static std::string _audioCodec = "aac";/// "pcm"
static int _audioChannels = 0;
static int _audioBitRate = 128;///kb
/////////////////////////////////////////////////////////
static std::string _videoCodec = "h264";
static int _videoWidth = 1920;
static int _videoHeight = 1080;
static int _videoFrameRate = 30;
static int _videoBitRate = 2000;///kb
///
static std::string _videoGopType = "ippp";///iiii,ippp,ibbp
static int _videoGop = 15;
static int _videoClock = 360000;
/////////////////////////////////////////////////////////
static bool _exit = false;
static std::string _path="./";
/////////////////////////////////////////////////////////
static int _diskLimit = 500;///MB
static int _fileLimit = 1000;
/////////////////////////////////////////////////////////
static bool FileListAdd(const char*url);
/////////////////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfRecorder(...): \n"
		 "record the video and audio streaming to a container file(*.mp4/*.mov). eg..\n"
		 "> omfRecorder -S30 -d300 -Caac -B128 -c h264 -w1920 -h1080 -f24 -b1000 -t ippp -g24 -K500 -F1000\n"
		},
		{"fname"	,'n', _fname			,"set the record file name."},
		{"duration"	,'d', _recordSeconds	,"set the record duration."},
		{"seamless"	,'S', _seamlessSeconds	,"set the seamless record duration per file."},
		{"protection",'P', _powerFailProtection	,"enable the power fail protection.Currently, "
													"only fragment mp4 file format can be used on this mode."},

		{"audio streaming:"},
		{"acodec"	,'C', _audioCodec		,"set the codec of audio."},
		{"abr"		,'B', _audioBitRate		,"set the bitrate(kb) of audio codec."},
		{"ach"		,'T', _audioChannels	,"set the target channels of audio codec."},
		{"video streaming:"},
		{"width"	,'w', _videoWidth		,"set the width of image."},
		{"height"	,'h', _videoHeight		,"set the height of image."},
		{"codec"	,'c', _videoCodec		,"set the codec of image."},
		{"fr"		,'f', _videoFrameRate	,"set the framerate of video streaming"},
		{"br"		,'b', _videoBitRate		,"set the bitrate(kb) of video streaming"},
		{"goptype"	,'t', _videoGopType		,"set the gop type of video encoder."},
		{"gop"		,'g', _videoGop			,"set the gop of video encoder."},
		{"clock"	,'o', _videoClock		,"set the clock of video encoder."},
		{"the files limit,auto delete old files when the disk free space or the file count is more than the limit."},
		{"disklimit",'K', _diskLimit		,"set the disk free space limit(MB)."},
		{"filelimit",'F', _fileLimit		,"set the record file number limit."},
		{},
};
////////////////////////////////////////////
static bool MessageProcess(const char* msg0){
	dbgTestPSL(msg0);
	OmfAttrSet ap(msg0);
	returnIfErrC(true,!ap);
	auto msg = ap.Get("msg");
	returnIfErrC(true,!msg);
	switch(Hash(msg)){
		case Hash("error"):
		case Hash("err"):
			dbgNotePSL("error");
			_exit=true;
			break;
		case Hash("eos"):
			dbgNotePSL("eos");
			_exit=true;
			break;
		case Hash("recordStart"):
			dbgTestPSL(ap.Get("url"));
			break;
		case Hash("recordStop"):
			break;
		case Hash("fileClose"):
			printf("msg fileClose \n");
			ap.Get("url");
			uint64 durMs;
			ap.Get("duration",durMs);
			ap.Get("filename");
			break;
		default:
			dbgTestPSL("unknow message:"<<msg);
			break;
	}
	return true;
}
static bool Process(bool _dbg){
	///
	std::string tssrc = (std::string)
		"type=Pipeline,name=plvsrc,layout={"
			"pv-yuv-src-adv:w="+_videoWidth+",h="+_videoHeight+
			"+FifoFilter:max=8"
			"+h264-encoder-dma:gop_type=ippp,gop=15,advancedMode=1,br=1000000,fr=30,name=enc"
			"+RingSink:max=8"
			"}"
   		"+type=pipeline,name=plasrc,layout={"
			"pv-pcm-src:name=pcm"
			"+FifoFilter:max=8"
			"+aac-encoder:name=aac,br=12000"
			"+RingSink:max=16"
			"}"
		"+type=Pipeline,name=ts,layout={"
			"BridgeSource:name=src0"
			"+FifoFilter:name=afifo,max=64"
			"+BridgeSource:name=src1,link=null"
			"+FifoFilter:name=vfifo,max=64"
   			"+ts-pack:name=ts,link={vfifo+afifo}"
			"+FifoSink:max=16"
		"},link={plasrc,plvsrc}"
  ;
	auto layout =(std::string)
		"type=Application,layout={"
			"type=Pipeline,layout={"
			 	"ShmSource:connect={"
					+tssrc+
	 			"},name=asrc"
	            "+FifoFilter:name=avfifo,max=64"
	       		"+RecordControler:name=rc,duration="+_recordSeconds+"s"
			    //"+muxer-mp4:name=mp4"
		   		"+muxer-hls:name=muxer,url=file://./%08d.ts,ts_duration=5s,ts_num_in_m3u8=4,ts_num_to_delete=10,remove_ts_when_close=false"
			 "}"
	 	"}"
	;
	dbgErrPSL(layout);
	OmfObject obj(layout);
	returnIfErrC(false,!obj);
	returnIfErrC(false,!obj.Register(&MessageProcess));
	returnIfErrC(false,!obj.StatusUp("play"));
	///
	while(!_exit){
		std::this_thread::sleep_for(100_ms);////wait 1s
	}
	///
	returnIfErrC(false,!obj.StatusUp("null"));
	return true;
}
/////////////////////////////////////////////////
static std::list<std::string> _filelist;

/////////////////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfRecorder(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	///process
	Process(OmfMain::Globle().DebugMode());
	///
	return 0;
}
