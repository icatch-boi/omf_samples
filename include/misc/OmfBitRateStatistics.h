//
// Created by jg.wang on 2020/9/25.
//
#pragma once

#include <list>
#include <chrono>

namespace omf{
class OmfBitRateStatistics {
public:
	typedef std::chrono::steady_clock::duration 	duration;
	typedef std::chrono::steady_clock::time_point   time_point;
	#define now() std::chrono::steady_clock::now()
public:
	OmfBitRateStatistics();
	OmfBitRateStatistics(duration dura);
public:
	typedef struct{
		time_point tp;
		int size;
	}item_t;
protected:
	std::list<item_t> _items;
	duration _duration=std::chrono::seconds(1);
	duration _dura=duration::zero();
	time_point _oldest;
	int _count=0;
	long long _total=0;
	int _bitRate=0;
	int _frameRate=0;
	bool _stable=false;
	bool _dbg=false;
public:
	void SetDuration(duration);
	void PushFrame(const time_point &tp,int size);
	void PushFrame(int size);
	void Clear();
	void SetDebug(bool);
public:
	int GetFrameCount()const;
	time_point GetOldest()const;
	duration GetDuration()const;
public:
	int GetBitRate()const;
	int GetFrameRate()const;
public:
	bool IsStable();
};
}


