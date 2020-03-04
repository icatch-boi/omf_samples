//
// Created by jg.wang on 2019/11/29.
//

#include "OmfRtspService.h"
#include "RtspService.h"
using namespace omf::rtsp;

OmfRtspService* OmfRtspService::CreateNew(const char*urlmap,const char*auth){
	return new RtspService(urlmap,auth);
}
