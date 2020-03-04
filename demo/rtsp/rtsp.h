//
// Created by jg.wang on 2019/11/28.
//

#pragma once
#include "IStreamControl.h"
#include "OmfAttrSet.h"
#include "OmfDbg.h"
#include "_typedef.h"
#include "_chrono_base.h"
#include "_hash.h"
#include "_uint.h"
#include "_chrono_base.h"
#include <memory>
#include <string>
using namespace omf;
using namespace omf::api;
using namespace omf::chrono;
namespace omf{
	namespace rtsp{
		using ISource = omf::api::streaming::common::IStreamOutput;
		using Packet = omf::api::streaming::common::frame_t;
		using OmfAttrSet = omf::api::OmfAttrSet;

		//#define R32(i,v) ((p[i])|((v)<<8))
		//inline uint16 Read16(const uint8* p) { return R32(0, R32(1, 0)); }
		//inline uint32 Read24(const uint8* p) { return R32(0, R32(1, R32(2, 0))); }
		//inline uint32 Read32(const uint8* p) { return R32(0, R32(1, R32(2, R32(3, 0)))); }
		//inline uint64 Read64(const uint8* p) { return Read32(p) | (((uint64)Read32(p + 4)) << 32); }
		//inline uint16 Read16BE(const uint8* p) { return R32(1, R32(0, 0)); }
		//inline uint32 Read24BE(const uint8* p) { return R32(3, R32(2, R32(1, 0))); }
		//inline uint32 Read32BE(const uint8* p) { return R32(3, R32(2, R32(1, R32(0, 0)))); }
		//inline uint64 Read64BE(const uint8* p) { return Read32BE(p + 4) | (((uint64)Read32BE(p)) << 32); }

		//inline uint16 Swap16(uint16 v) { return (v << 8) | (v >> 8); }
		//inline uint32 Swap24(uint32 v) { return (v << 24 >> 8) | (v & 0xff00ff00) | (v << 8 >> 24); }
		//inline uint32 Swap32(uint32 v) { return (v << 24) | (v >> 24) | (v >> 8 << 24 >> 8) | (v << 8 >> 24 << 8); }
		//inline uint64 Swap64(uint64 v) { return Read64BE((uint8*)&v); }
		//inline uint16 Swap(uint16 v) { return Swap16(v);}
		//inline uint32 Swap(uint32 v) { return Swap32(v);}
		//inline uint64 Swap(uint64 v) { return Swap64(v);}

		//static constexpr inline long long toSeconds(const std::chrono::steady_clock::duration &this_) {
		//	return std::chrono::duration_cast<std::chrono::seconds>(this_).count();
		//}
		//static constexpr inline long long toUs(const std::chrono::steady_clock::duration &this_) {
		//	return std::chrono::duration_cast<std::chrono::microseconds>(this_).count();
		//}
		//static constexpr inline long long toMs(const std::chrono::steady_clock::duration &this_) {
		//	return std::chrono::duration_cast<std::chrono::milliseconds>(this_).count();
		//}
		//static constexpr inline struct timeval toTimeVal(const std::chrono::steady_clock::duration &this_) {
		//	return timeval{(time_t)toSeconds(this_),(time_t)(toUs(this_)%1000000)};
		//}

		//static constexpr inline long long toSeconds(const std::chrono::steady_clock::time_point &this_) {
		//	return toSeconds(this_.time_since_epoch());
		//}
		//static constexpr inline long long toUs(const std::chrono::steady_clock::time_point &this_) {
		//	return toUs(this_.time_since_epoch());
		//}
		//static constexpr inline long long toMs(const std::chrono::steady_clock::time_point &this_) {
		//	return toMs(this_.time_since_epoch());
		//}
		//static constexpr inline struct timeval toTimeVal(const std::chrono::steady_clock::time_point &this_) {
		//	return toTimeVal(this_.time_since_epoch());
		//}

	}
}