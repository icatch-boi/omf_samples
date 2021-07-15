//
// Created by jg.wang on 2020/10/30.
//
#pragma once

#include "_enum.h"
#include <string>
namespace omf {
	enum class IOMode:unsigned{
		null = 0,
		pull = Convert4low("pull"),
		push = Convert4low("push"),
	};
	///
	std::string to_string(IOMode t);
	bool from_string(const char*s,IOMode &t);
	///
	class OmfIOMode {
	public:
		OmfIOMode(IOMode mode = IOMode::pull);
		OmfIOMode(const char*);
	protected:
		IOMode _mode;
	public:
		bool operator=(IOMode mode)const;
		operator IOMode()const;
		operator bool()const;
	public:
		IOMode Get() const;
		bool Set(IOMode);
	public:
		bool PullMode(bool pull);
		bool PullMode() const;
		bool PushMode(bool push);
		bool PushMode() const;
	public:
		std::string ToString()const;
		bool ToString(std::string&)const;
		bool FromString(const char *);
	};
}
