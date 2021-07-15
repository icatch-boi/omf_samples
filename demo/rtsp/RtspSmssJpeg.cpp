#include "RtspSmssJpeg.h"
#include "_base.h"
#include <string.h>
using namespace NAMESPACE_ICATCH_PLUGIN::rtsp;

DEFINE_REGISTER(RtspSmssJpeg) {
	TClass<UsageEnvironment*,PacketRing*>::Register<RtspSmssJpeg>(
		 "rtsp-smss-jpeg-live555", {
		})->Init(LIBRARY, VERSION(0), "iCatchtek", "jg.wang");
}
RtspSmssJpeg::RtspSmssJpeg(UsageEnvironment *env,PacketRing* ring)
	:RtspSmss(env,ring)
	,_width(0)
	,_height(0)
	,_qtableIndex(0)
	,_qtablePrecision(0)
	,_qtableOffset(0)
	,_qtableLen(0)
	,_qtableCnt(0)
	,_vlcoffset(0)
	,_sampleType(0)//0:422 1:420
	,_qtablePtr(0)
	,_parsed(false)
	,_index(0)
{
	_type="video";
	_codec="JPEG";
	_rate=90000;
	_payloadtype=26;
	_channels=1;
}
bool RtspSmssJpeg::Parse(){
	IMedia* media = _ring->Media();
	returnIfErrC(false, !media);
	returnIfErrC(false, !media->IsCodec("jpeg"));
	returnIfErrC(false, !media->Get("width", _width));
	returnIfErrC(false, !media->Get("height", _height));
	returnIfErrC(false, !media->Get("parser", _parser));
	returnIfErrC(false, !_width);
	returnIfErrC(false, !_height);
	returnIfErrC(false, !_parser);
	_parsed = false;
	return true;
}

bool RtspSmssJpeg::FramePreProcess(uint8*&ptr,int& size){
	if(!_parsed) {
		IMedia* media = _ring->Media();
		returnIfErrC(false, !media);
		returnIfErrC(false, !media->IsCodec("jpeg"));
		object* obj = _parser(ptr,size);
		returnIfErrC(false, !obj);
		returnIfErrC(false, !obj->Get("width",_width));
		returnIfErrC(false, !obj->Get("height",_height));
		returnIfErrC(false, !obj->Get("qtblidx",_qtableIndex));
		returnIfErrC(false, !obj->Get("qtblprecision",_qtablePrecision));
		returnIfErrC(false, !obj->Get("qtbloffset",_qtableOffset));
		returnIfErrC(false, !obj->Get("qtbllen",_qtableLen));
		returnIfErrC(false, !obj->Get("qtblcnt",_qtableCnt));
		returnIfErrC(false, !obj->Get("vlcoffset",_vlcoffset));
		returnIfErrC(false, !obj->Get("fmt",_sampleType));
		delete obj;
		_parsed = true;
	}
	_qtablePtr = 0;
	if(_qtableIndex>=128)
		_qtablePtr=ptr+_qtableOffset;
	ptr+=_vlcoffset;
	size-=_vlcoffset;
	_index=0;

	return true;
}
int RtspSmssJpeg::RtpHeadUpdate(uint8*ptr,int& size){
	ptr[0] = 0; // Type-specific
	ptr[1] = (_index >> 16)& 0xff;
	ptr[2] = (_index >> 8) & 0xff;
	ptr[3] = _index		   & 0xff;
	ptr[4] = _sampleType==422?0:1;;//type;
	ptr[5] = _qtableIndex;//qtab;//source->get(DEVICE__VIDEO__JPEG_QTableIndex);//qFactor();
	ptr[6] = _width>>3;//width();
	ptr[7] = _height>>3;//height();
	ptr+=8;
	int headsize=0;//_headsize;
	//_print("%d,%d\n",_index,len);
	if(!_index && _qtableIndex>=128){//dqt
		ptr[0] = 0;//MBZ
		ptr[1] = _qtablePrecision;
		ptr[2] = _qtableLen >> 8;
		ptr[3] = _qtableLen & 0xff;
		ptr+=4;
		uint8* src=_qtablePtr+1;
		int tlen = _qtableLen/_qtableCnt;
		int i;
		for(i=0;i<_qtableCnt;i++,ptr+=tlen,src+=tlen+1)
			memcpy(ptr,src,tlen);
		headsize+=_qtableLen+4;
	}
	_index+=_packsize-headsize-_headsize;
	return headsize;
}
