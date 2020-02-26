//
// Created by jg.wang on 2019/11/29.
//

#include "IRtspService.h"
#include "RtspService.h"
using namespace omf::rtsp;

IRtspService* IRtspService::CreateNew(const char*urlmap,const char*auth){
	return new RtspService(urlmap,auth);
}
