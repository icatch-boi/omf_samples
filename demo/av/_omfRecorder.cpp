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
static int _recordSeconds=60;
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
			FileListAdd(ap.Get("url"));
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
	auto fname = std::string("file://")+_fname;
	OmfAttrSet pat(fname);
	auto muxer = pat.Get("ext");
	returnIfErrC(false,!muxer);
	auto path = pat.Get("path");
	returnIfErrC(false,!path);
	_path=path;
	if(_powerFailProtection){
		returnIfErrC(false,::Hash(muxer)!=::Hash("mp4"));
		muxer = "fmp4";
	}
	///
	auto layout =(std::string)
		"type=Application,layout={"
			"type=Pipeline,layout={"
				"ShmSource:connect={"
					"type=pipeline,name=plAacSource,layout={"
						"pv-pcm-src:name=pcm"
						"+encoder-"+_audioCodec+":br="+_audioBitRate*1000+",ch="+_audioChannels+
						"+FifoSink:max=16"
					"}"
				"},name=asrc"
				"+FifoFilter:name=afifo,max=64"
				//
				"+ShmSource:connect={"
					"type=pipeline,layout={"
						"pv-yuv-src-adv:w="+_videoWidth+",h="+_videoHeight+
						"+FrameRateCtrl:name=frc,fr="+_videoFrameRate+
						"+encoder-"+_videoCodec+":"
							"goptype="+_videoGopType+
							",gop="+_videoGop+
							",br="+_videoBitRate*1000+
							",fr="+_videoFrameRate+
							",clock="+_videoClock+
						"+FifoSink:max=2"
					"},name=plPvH264RecSrc"
				"},name=vsrc,link=null"
				"+FifoFilter:name=vfifo,max=64"
				//
				"+Sync:name=sync,link={afifo,vfifo}"
				"+RecordControler:name=rc,duration="+_recordSeconds+"s"
				"+Seamless:name=seamless,duration="+_seamlessSeconds+"s,dbg="+_dbg+
				"+muxer-"+muxer+":name=muxer,flush="+_powerFailProtection+
					",urlcreaterlayout={type=UrlRtc,pattern="+fname+"}"
					",sendmsg={recordStart={oper=AfterOpen,send={upward}},recordStop={oper=AfterClose,send={upward}}}"
			"}"
		"}"
	;
	dbgTestPSL(layout);
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
static bool RemoveFile(){
	std::string fname = _filelist.front();
	_filelist.pop_front();
	returnIfErrC(false,!fname.empty());
	dbgTestPSL(fname.c_str());
	int rst = remove(fname.c_str());
	returnIfErrCS(false,rst,rst);
	return true;
}
static bool RemoveFile(const char*path,int limit){
	struct statfs diskInfo;

	statfs(path, &diskInfo);
	unsigned long long blocksize = diskInfo.f_bsize; //每个block里包含的字节数
	unsigned long long totalsize = blocksize * diskInfo.f_blocks; //总的字节数，f_blocks为block的数目
	printf("Total_size = %llu B = %llu KB = %llu MB = %llu GB\n",
		   totalsize, totalsize>>10, totalsize>>20, totalsize>>30);

	unsigned long long freeDisk = diskInfo.f_bfree * blocksize; //剩余空间的大小
	printf("Disk_free = %llu MB = %llu GB\n",
		   freeDisk>>20, freeDisk>>30);

	unsigned long long availableDisk = diskInfo.f_bavail * blocksize; //可用空间大小
	printf("Disk_available = %llu MB = %llu GB\n",
		   availableDisk>>20, availableDisk>>30);

	///find first file
	if(freeDisk<limit) {
		if(_filelist.empty()){
			dbgTestPSL("filelist is empty!");
			return false;
		}else{
			return RemoveFile();
		}
	}
	return false;
}
static bool RemoveFileList(){
	if(_fileLimit){
		while(_filelist.size()>_fileLimit){
			RemoveFile();
		}
	}
	///
	while(RemoveFile(_path.c_str(),_diskLimit*1000000));
	return true;
}
static bool FileListAdd(const char*url){
	dbgTestPSL(url);
	OmfAttrSet obj(url);
	returnIfErrC(false, !obj);
	const char *path = obj.Get("path");
	returnIfErrC(false, !path);

	dbgTestPSL(path);
	_filelist.push_back(path);
	RemoveFileList();
	return true;
}
static bool FileListInit(const char*path){
	return true;
}
////////////////////////////////////////////
static bool Check(){
	returnIfErrC(false,!_fname);
	return true;
}
/////////////////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfRecorder(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	returnIfErrC(0,!Check());
	///process
	Process(OmfMain::Globle().DebugMode());
	///
	return 0;
}
