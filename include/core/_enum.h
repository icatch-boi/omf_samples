#pragma once
#include "_typedef.h"
#include "_string_func.h"

namespace omf{

constexpr uint32 Convert32(uint32 v, const char s) {
	return (v << 8) | s;
}
constexpr uint32 Convert32(uint32 v, const char*s, int i, int c) {
	return (*s && i < c) ? Convert32(Convert32(v, s + 1, i + 1, c), *s) : 0;
}
constexpr uint32 Convert32(const char*s, int c = 4) {
	return s ? Convert32(0, s, 0, c) : 0;
}
constexpr uint64 Convert64(uint64 v, const char s) {
	return (v << 8) | s;
}
constexpr uint64 Convert64(uint64 v, const char*s, int i, int c) {
	return (*s && i < c) ? Convert64(Convert64(v, s + 1, i + 1, c), *s) : 0;
}
constexpr uint64 Convert64(const char*s, int c = 8, char f = 0) {
	return (s ? Convert64(0, s, 0, c) : 0) | (((uint64)f) << 56);
}

constexpr uint32 Convert32up(uint32 v, const char s) {
	return (v << 8) | Upper(s);
}
constexpr uint32 Convert32up(uint32 v, const char*s, int i, int c) {
	return (*s && i < c) ? Convert32up(Convert32up(v, s + 1, i + 1, c), *s) : 0;
}
constexpr uint32 Convert32up(const char*s, int c = 4) {
	return s ? Convert32up(0, s, 0, c) : 0;
}
constexpr uint64 Convert64up(uint64 v, const char s) {
	return (v << 8) | Upper(s);
}
constexpr uint64 Convert64up(uint64 v, const char*s, int i, int c) {
	return (*s && i < c) ? Convert64up(Convert64up(v, s + 1, i + 1, c), *s) : 0;
}
constexpr uint64 Convert64up(const char*s, int c = 8, char f = 0) {
	return (s ? Convert64up(0, s, 0, c) : 0) | (((uint64)f) << 56);
}

constexpr uint32 Convert32low(uint32 v, const char s) {
	return (v << 8) | Lower(s);
}
constexpr uint32 Convert32low(uint32 v, const char*s, int i, int c) {
	return (*s && i < c) ? Convert32low(Convert32low(v, s + 1, i + 1, c), *s) : 0;
}
constexpr uint32 Convert32low(const char*s, int c = 4) {
	return s ? Convert32low(0, s, 0, c) : 0;
}
constexpr uint64 Convert64low(uint64 v, const char s) {
	return (v << 8) | Lower(s);
}
constexpr uint64 Convert64low(uint64 v, const char*s, int i, int c) {
	return (*s && i < c) ? Convert64low(Convert64low(v, s + 1, i + 1, c), *s) : 0;
}
constexpr uint64 Convert64low(const char*s, int c = 8, char f = 0) {
	return (s ? Convert64low(0, s, 0, c) : 0) | (((uint64)f) << 56);
}

constexpr uint32 Convert3(char const* s) { return Convert32(s, 3); }
constexpr uint32 Convert4(char const* s) { return Convert32(s, 4); }
constexpr uint64 Convert6(char const* s, char f = 0) { return Convert64(s, 6, f); }
constexpr uint64 Convert7(char const* s, char f = 0) { return Convert64(s, 7, f); }
constexpr uint64 Convert8(char const* s) { return Convert64(s, 8); }

constexpr uint32 Convert3up(char const* s) { return Convert32up(s, 3); }
constexpr uint32 Convert4up(char const* s) { return Convert32up(s, 4); }
constexpr uint64 Convert6up(char const* s, char f = 0) { return Convert64up(s, 6, f); }
constexpr uint64 Convert7up(char const* s, char f = 0) { return Convert64up(s, 7, f); }
constexpr uint64 Convert8up(char const* s) { return Convert64up(s, 8); }

constexpr uint32 Convert3low(char const* s) { return Convert32low(s, 3); }
constexpr uint32 Convert4low(char const* s) { return Convert32low(s, 4); }
constexpr uint64 Convert6low(char const* s, char f = 0) { return Convert64low(s, 6, f); }
constexpr uint64 Convert7low(char const* s, char f = 0) { return Convert64low(s, 7, f); }
constexpr uint64 Convert8low(char const* s) { return Convert64low(s, 8); }

constexpr uint64 Convert64val(uint64 v) { return v << 8 >> 8; }
constexpr char Convert64flag(uint64 v) { return ((char*)&v)[7]; }
#define Convert64str(v) ((const char*)&v)
#define Convert32str(v) ((const char*)&v)

constexpr uint32 Convert4cc(char a, char b = 0, char c = 0, char d = 0) {
	return ((unsigned)a) | (((unsigned)b) << 8) | (((unsigned)c) << 16) | (((unsigned)d) << 24);
}
///fcc
constexpr uint32 FourCC(char a, char b = 0, char c = 0, char d = 0) {
	return ((unsigned)(a)) | (((unsigned)(b)) << 8) | (((unsigned)(c)) << 16) | (((unsigned)(d)) << 24);
}
constexpr uint32 FourCC(const char* s) {
	return Convert32(s, 4);
}
#define FCC FourCC

#define ENUM4(x) x=::Convert4(#x)
}
