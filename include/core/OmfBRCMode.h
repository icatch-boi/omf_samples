//
// Created by jg.wang on 2020/10/30.
//
#pragma once

#include "_enum.h"
#include <string>
namespace omf {
	enum class BRCMode:unsigned {
		null = 0,
		mbrc = Convert4low("mbrc"),
		cbrc = Convert4low("cbrc"),
		vbrc = Convert4low("vbrc"),
		abrc = Convert4low("abrc"),
	};
	std::string to_string(BRCMode t);
	bool from_string(const char*s,BRCMode &t);

	class OmfBRCMode {
	public:
		OmfBRCMode(BRCMode mode=BRCMode::null);
		OmfBRCMode(const char*);
	protected:
		BRCMode _mode;
	public:
		bool operator==(BRCMode mode)const;
		OmfBRCMode& operator=(BRCMode mode);
		operator BRCMode()const;
		operator bool()const;
	public:
		bool Is(BRCMode)const;
	public:
		BRCMode Get()const;
		bool Set(BRCMode);
	public:
		std::string ToString()const;
		bool FromString(const char*);
	};

}
