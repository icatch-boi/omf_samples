#pragma once
#include "_typedef.h"
#include <stdlib.h>
#include <string>
#include <typeinfo>
namespace omf{
/// lower upper
constexpr inline char Lower(char c) { return (c <= 'Z' && c >= 'A') ? (c + 'a' - 'A') : c; };
constexpr inline char Upper(char c) { return (c <= 'z' && c >= 'a') ? (c + 'A' - 'a') : c; };

void ToLower(char*);
void ToUpper(char*);
bool Replace(char*, char, char);
bool Replace(char*, char*, char);
char* Strdup(const char*);
char* StrdupTo(const char*,char);
//#if _GLIBCXX_USE_C99 || _GLIBCXX_USE_C99_STDIO

using std::to_string;
std::string to_string(void*v);
static inline std::string to_string(const char*s){return s?s:"nullptr";}
static inline std::string to_string(char*s){return s?s:"nullptr";}
static inline std::string to_string(const std::string&s){return s;}
template<class T>
static inline std::string to_string(T*v){return to_string((void*)v);}
//static inline std::string to_string(unsigned long long t) {return std::to_string(t);}
//static inline std::string to_string(long long t)          {return std::to_string(t);}
//static inline std::string to_string(unsigned int t)       {return std::to_string(t);}
//static inline std::string to_string(int t)                {return std::to_string(t);}
//static inline std::string to_string(unsigned long t)      {return std::to_string(t);}
//static inline std::string to_string(long t)               {return std::to_string(t);}
static inline std::string to_string(unsigned short t)       {return std::to_string(t);}
static inline std::string to_string(short t)                {return std::to_string(t);}
static inline std::string to_string(unsigned char t)        {return std::to_string(t);}
static inline std::string to_string(char t)                 {return std::to_string(t);}
static inline std::string to_string(bool t)                 {return t?"true":"false";}


static inline std::string operator+(const std::string&str,char*  v){return str+to_string(v);}
static inline std::string operator+(const std::string&str,const char* v){return str+to_string(v);}
//static inline std::string operator+(const std::string&str,std::string& v){return str+to_string(v);}
static inline std::string operator+(const std::string&str,int8   v){return str+to_string(v);}
static inline std::string operator+(const std::string&str,int16  v){return str+to_string(v);}
static inline std::string operator+(const std::string&str,int32  v){return str+to_string(v);}
static inline std::string operator+(const std::string&str,int64  v){return str+to_string(v);}
static inline std::string operator+(const std::string&str,uint8  v){return str+to_string(v);}
static inline std::string operator+(const std::string&str,uint16 v){return str+to_string(v);}
static inline std::string operator+(const std::string&str,uint32 v){return str+to_string(v);}
static inline std::string operator+(const std::string&str,uint64 v){return str+to_string(v);}
static inline std::string operator+(const std::string&str,bool   v){return str+(v?"true":"false");}
template<class T=void>
static inline std::string operator+(const std::string&str,T* v){return str+to_string(v);}
///
//static inline std::string& operator+=(std::string&str,char*  v){return str+=to_string(v);}
//static inline std::string& operator+=(std::string&str,const char* v){return str+=to_string(v);}
//static inline std::string& operator+=(std::string&str,const std::string& v){return str+=to_string(v);}
//static inline std::string& operator+=(std::string&str,int8   v){return str+=to_string(v);}
static inline std::string& operator+=(std::string&str,int16  v){return str+=to_string(v);}
static inline std::string& operator+=(std::string&str,int32  v){return str+=to_string(v);}
static inline std::string& operator+=(std::string&str,int64  v){return str+=to_string(v);}
static inline std::string& operator+=(std::string&str,uint8  v){return str+=to_string(v);}
static inline std::string& operator+=(std::string&str,uint16 v){return str+=to_string(v);}
static inline std::string& operator+=(std::string&str,uint32 v){return str+=to_string(v);}
static inline std::string& operator+=(std::string&str,uint64 v){return str+=to_string(v);}
static inline std::string& operator+=(std::string&str,bool   v){return str+=(v?"true":"false");}
template<class T=void>
static inline std::string& operator+=( std::string&str,T* v){return str+=to_string(v);}


static inline unsigned atou(const char*s){return strtoul(s,0,0);}

template<class T>
static inline auto from_string(const char*s,T&t)->typename std::enable_if<std::is_integral<T>::value,void>::type
{t = (T)strtoll(s,0,0);}
template<class T>
static inline auto from_string(const char*s,T&t)->typename std::enable_if<!std::is_integral<T>::value,void>::type
{t = (T)s;}
//#endif

//static inline std::string& operator+=(std::string&str,int8  v){return str+=std::to_string(v);}
//static inline std::string& operator+=(std::string&str,int16 v){return str+=std::to_string(v);}
//static inline std::string& operator+=(std::string&str,int32 v){return str+=std::to_string(v);}
//static inline std::string& operator+=(std::string&str,int64 v){return str+=std::to_string(v);}
//static inline std::string& operator+=(std::string&str,uint8  v){return str+=std::to_string(v);}
//static inline std::string& operator+=(std::string&str,uint16 v){return str+=std::to_string(v);}
//static inline std::string& operator+=(std::string&str,uint32 v){return str+=std::to_string(v);}
//static inline std::string& operator+=(std::string&str,uint64 v){return str+=std::to_string(v);}
//static inline std::string& operator+=(std::string&str,bool v){return str+=(v?"true":"false");}
///
static inline bool operator!(const std::string&s){return s.empty();}
///
const char* TypeName(const char *src);
const char* TypeName(const std::type_info& type);
}
