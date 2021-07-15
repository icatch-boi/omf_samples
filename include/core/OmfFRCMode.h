//
// Created by jg.wang on 2020/10/30.
//
#pragma once

#include "_enum.h"
#include <string>
namespace omf {
	enum class FRCMode:unsigned {
		null = 0,
		vfrc = Convert4low("vfrc"),
		afrc = Convert4low("afrc"),
		trig = Convert4low("trig"),
	};
	std::string to_string(FRCMode t);
	bool from_string(const char*s,FRCMode &t);

	class OmfFRCMode {
	public:
		OmfFRCMode(FRCMode mode=FRCMode::null);
		OmfFRCMode(const char*);
	protected:
		FRCMode _mode;
	public:
		bool operator=(FRCMode mode)const;
		OmfFRCMode& operator=(FRCMode mode);
		operator FRCMode()const;
		operator bool()const;
	public:
		bool Is(FRCMode)const;
		bool Is(const char*)const;
		bool Is(std::initializer_list<FRCMode>)const;
	public:
		FRCMode Get()const;
		bool Set(FRCMode);
	public:
		std::string ToString()const;
		bool FromString(const char*);
	};

}
