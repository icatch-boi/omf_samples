//
// Created by jg.wang on 2020/4/27.
//
#pragma once

#include "IStreamControl.h"
#include <vector>
namespace omf {
	namespace api {
		namespace streaming {
			class IDemuxer
				:public common::IStreamControl
			{
			public:
				using ISource=common::IStreamOutput;
			public:
				virtual bool Url(const char*) =0;
				virtual const char* Url()const =0;

			public:
				virtual bool RegisterOutputCallback(std::function<void(ISource*)>)=0;
				virtual std::vector<ISource*> Outputs()const=0;

			public:
				/**
				 * create a new OmfDemuxer instance. \n
				 * @param keywords[in] the keywords for selecting a OmfDemuxer type. \n
				 * @return the new OmfDemuxer instance. \n
				 */
				static IDemuxer* CreateNew(const char *);
			};
		}
	}
}
