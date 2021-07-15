//
// Created by jg.wang on 2020/4/27.
//
#pragma once

#include "IStreamControl.h"
#include <vector>
namespace omf {
	namespace api {
		namespace streaming {
			class IHlsSource
				:public virtual common::IStreamControl
			{
			public:
				/**
				 * create a new OmfDemuxer instance. \n
				 * @param keywords[in] the keywords for selecting a OmfDemuxer type. \n
				 * @return the new OmfDemuxer instance. \n
				 */
				virtual bool SetVideoCodec(const char*)=0;
				virtual bool SetAudioCodec(const char*)=0;
				virtual bool SetTsDuration(int dur)=0;
				virtual bool SetTsUrlPattern(const char*)=0;
				virtual bool SetM3u8UrlPattern(const char*)=0;
				virtual bool SetFilePath(const char*)=0;
				virtual bool EnablePrerec(int)=0;
				static IHlsSource* CreateNew(const char *keywords);
			};
		}
	}
}
