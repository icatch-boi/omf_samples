#pragma once
#include <string>
#include "_hash.h"
namespace omf{
////////////////////////////////////////////////////////////////////////////////
template<class T>inline bool Equal(const T* t, const T *p) {return (void*)t == (void*)p;}
template<class T>inline bool Equal(T* t, const T *p) {return (void*)t == (void*)p;}
template<class T>inline bool Equal(const T* t, T *p) {return (void*)t == (void*)p;}
template<class T>inline bool Equal(T* t, T *p) {return (void*)t == (void*)p;}
template<class T>inline bool Equal(const T& t, const T *p) {return (void*)&t == (void*)p;}
template<class T>inline bool Equal(const T* t, const T &p) {return (void*)t == (void*)&p;}
template<class T, class P>inline bool Equal(const T& t, const P &p) {return t == p;}

template<class T, class P, class ...Ps>
inline bool Equal(const T& t, const P &p, Ps... ps) {return Equal(t, p) && Equal(t, ps...);}

inline bool Equal(const char* t, const char*p) {return ::Hash(t)==::Hash(p);}
inline bool Equal(char* t, char*p) {return ::Hash(t)==::Hash(p);}
inline bool Equal(const char* t, char*p) {return ::Hash(t)==::Hash(p);}
inline bool Equal(char* t, const char*p) {return ::Hash(t)==::Hash(p);}

inline bool Equal(const std::string& t, char*p) {return ::Hash(t)==::Hash(p);}
inline bool Equal(const std::string& t, const char*p) {return ::Hash(t)==::Hash(p);}
inline bool Equal(const std::string& t, const std::string& p) {return ::Hash(t)==::Hash(p);}
inline bool Equal(const char* t, const std::string& p) {return ::Hash(t)==::Hash(p);}
inline bool Equal(char* t, const std::string& p) {return ::Hash(t)==::Hash(p);}

}
