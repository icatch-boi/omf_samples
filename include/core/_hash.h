#pragma once
#include "_string_func.h"
#include <string>

namespace omf{
const unsigned _FNV_offset_basis = 2166136261U;//0x811c9dc5
const unsigned _FNV_prime = 16777619U;//0x01000193
constexpr inline unsigned Hash(unsigned v, char s) {
	return (v ^ (unsigned)Lower(s))*_FNV_prime;
}
///
constexpr inline unsigned Hash(unsigned v, const char* s) {
	return *s ? Hash(Hash(v, *s), s + 1) : v;
}
constexpr inline unsigned Hash(const char* s) {
	return s?Hash(_FNV_offset_basis, s):0;
}
///
constexpr inline unsigned Hash(unsigned v, const char* s, char end) {
	return (*s&&*s!=end) ? Hash(Hash(v, *s), s + 1,end) : v;
}
constexpr inline unsigned Hash(const char* s,char end) {
	return s?Hash(_FNV_offset_basis, s,end):0;
}
///
constexpr inline unsigned Hash(unsigned v, const char* s, int max) {
	return (*s && max) ? Hash(Hash(v, *s), s + 1,max-1) : v;
}
constexpr inline unsigned Hash(const char* s,int max) {
	return (s && max)?Hash(_FNV_offset_basis, s,max):0;
}
///
constexpr inline unsigned Hash(unsigned v, const char* s, char end, int max) {
	return (*s && max &&*s!=end) ? Hash(Hash(v, *s), s + 1, end, max-1) : v;
}
constexpr inline unsigned Hash(const char* s,char end, int max) {
	return (s && max)?Hash(_FNV_offset_basis, s, end, max):0;
}
///
inline unsigned Hash(const std::string& t) {
	return Hash(t.c_str());
}
inline unsigned Hash(const std::string& t, char end) {
	return Hash(t.c_str(), end);
}
inline unsigned Hash(const std::string& t, int max) {
	return Hash(t.c_str(), max);
}
inline unsigned Hash(const std::string& t, char end, int max) {
	return Hash(t.c_str(), end, max);
}
///
unsigned HashRef(char const* &s, char end);
unsigned HashRef(char const* &s);
inline unsigned HashRef(char * &s, char end){return HashRef((char const*&)s,end);}
inline unsigned HashRef(char * &s){return HashRef((char const*&)s);}

inline unsigned HashLoop(char const* s, char end){return HashRef(s,end);}
inline unsigned HashLoop(char const* s){return HashRef(s);}
inline unsigned HashLoop(char * s, char end){return HashRef(s,end);}
inline unsigned HashLoop(char * s){return HashRef(s);}
}

