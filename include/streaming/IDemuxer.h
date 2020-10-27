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
				:public virtual common::IStreamControl
			{
			public:
				using ISource=common::IStreamSource;
			public:
				virtual bool Url(const char*) =0;
				virtual const char* Url()const =0;
				virtual void Live(bool) = 0;
				virtual bool Live() = 0;
			public:
				virtual bool RegisterOutputCallback(std::function<void(ISource*)>)=0;
				virtual std::vector<ISource*> Outputs()const=0;

			public:
				/**
				 * create a new OmfDemuxer instance. \n
				 * @param keywords[in] the keywords for selecting a OmfDemuxer type. \n
				 * @return the new OmfDemuxer instance. \n
				 */
				static IDemuxer* CreateNew(const char *keywords);
				static IDemuxer* CreateNewFromConfig(const char *config);
				static IDemuxer* CreateNewFromFile(const char *file);
			};
		}
	}
}
