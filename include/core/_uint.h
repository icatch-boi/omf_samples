#pragma once
#include "_typedef.h"
namespace omf{
////////////////////////////////////////////////////////////////
///
#define R32(i,v) ((p[i])|((v)<<8))
inline uint16 Read16(const uint8* p) { return R32(0, R32(1, 0)); }
inline uint32 Read24(const uint8* p) { return R32(0, R32(1, R32(2, 0))); }
inline uint32 Read32(const uint8* p) { return R32(0, R32(1, R32(2, R32(3, 0)))); }
inline uint64 Read64(const uint8* p) { return Read32(p) | (((uint64)Read32(p + 4)) << 32); }
inline uint16 Read16BE(const uint8* p) { return R32(1, R32(0, 0)); }
inline uint32 Read24BE(const uint8* p) { return R32(3, R32(2, R32(1, 0))); }
inline uint32 Read32BE(const uint8* p) { return R32(3, R32(2, R32(1, R32(0, 0)))); }
inline uint64 Read64BE(const uint8* p) { return Read32BE(p + 4) | (((uint64)Read32BE(p)) << 32); }
inline void Write16(uint8* p, uint16 v) { uint8* s = (uint8*)&v; p[0] = s[0]; p[1] = s[1]; }
inline void Write24(uint8* p, uint32 v) { uint8* s = (uint8*)&v; p[0] = s[0]; p[1] = s[1]; p[2] = s[2]; }
inline void Write32(uint8* p, uint32 v) { uint8* s = (uint8*)&v; p[0] = s[0]; p[1] = s[1]; p[2] = s[2]; p[3] = s[3]; }
inline void Write64(uint8* p, uint64 v) { uint32* s = (uint32*)&v; Write32(p, s[0]); Write32(p + 4, s[1]); }
inline void Write16BE(uint8*p, uint16 v) { uint8* s = (uint8*)&v; p[0] = s[1]; p[1] = s[0]; }
inline void Write24BE(uint8*p, uint32 v) { uint8* s = (uint8*)&v; p[0] = s[2]; p[1] = s[1]; p[2] = s[0]; }
inline void Write32BE(uint8*p, uint32 v) { uint8* s = (uint8*)&v; p[0] = s[3]; p[1] = s[2]; p[2] = s[1]; p[3] = s[0]; }
inline void Write64BE(uint8*p, uint64 v) { uint32* s = (uint32*)&v; Write32BE(p, s[1]);	Write32BE(p + 4, s[0]); }
inline void Write(uint8* p, uint16 v) { uint8* s = (uint8*)&v; p[0] = s[0]; p[1] = s[1]; }
inline void Write(uint8* p, uint32 v) { uint8* s = (uint8*)&v; p[0] = s[0]; p[1] = s[1]; p[2] = s[2]; p[3] = s[3]; }
inline void Write(uint8* p, uint64 v) { uint32* s = (uint32*)&v; Write32(p, s[0]); Write32(p + 4, s[1]); }
inline void WriteBE(uint8*p, uint16 v) { uint8* s = (uint8*)&v; p[0] = s[1]; p[1] = s[0]; }
inline void WriteBE(uint8*p, uint32 v) { uint8* s = (uint8*)&v; p[0] = s[3]; p[1] = s[2]; p[2] = s[1]; p[3] = s[0]; }
inline void WriteBE(uint8*p, uint64 v) { uint32* s = (uint32*)&v; Write32BE(p, s[1]);	Write32BE(p + 4, s[0]); }
///
inline uint16 Swap16(uint16 v) { return (v << 8) | (v >> 8); }
inline uint32 Swap24(uint32 v) { return (v << 24 >> 8) | (v & 0xff00ff00) | (v << 8 >> 24); }
inline uint32 Swap32(uint32 v) { return (v << 24) | (v >> 24) | (v >> 8 << 24 >> 8) | (v << 8 >> 24 << 8); }
inline uint64 Swap64(uint64 v) { return Read64BE((uint8*)&v); }
inline uint16 Swap(uint16 v) { return Swap16(v);}
inline uint32 Swap(uint32 v) { return Swap32(v);}
inline uint64 Swap(uint64 v) { return Swap64(v);}
///
inline void* SwapCpy(uint16*dst,const uint16*src,int size){while(size>0){*dst++=Swap(*src++);size-=2;}return dst;}
inline void* SwapCpy(uint32*dst,const uint32*src,int size){while(size>0){*dst++=Swap(*src++);size-=4;}return dst;}
inline void* Swap16Cpy(uint16*dst,const uint16*src,int size){return SwapCpy(dst,src,size);}
inline void* Swap32Cpy(uint32*dst,const uint32*src,int size){return SwapCpy(dst,src,size);}
//inline void* SwapCpy(uint16*dst,const uint16*src,int size){while(size>0){*dst++=Swap(*src++);size-=2;}return dst;}
//inline void* SwapCpy(uint32*dst,const uint32*src,int size){while(size>0){*dst++=Swap(*src++);size-=4;}return dst;}
//inline void* Swap16Cpy(uint16*dst,const uint16*src,int size){return SwapCpy(dst,src,size);}
//inline void* Swap32Cpy(uint32*dst,const uint32*src,int size){return SwapCpy(dst,src,size);}
/////////////////////////////////////////////////////////////////
inline uint16 Read16ref(uint8*&p){auto v=Read16(p);p+=2;return v;}
inline uint32 Read24ref(uint8*&p){auto v=Read24(p);p+=3;return v;}
inline uint32 Read32ref(uint8*&p){auto v=Read32(p);p+=4;return v;}
inline uint64 Read64ref(uint8*&p){auto v=Read64(p);p+=8;return v;}
inline uint16 Read16refBE(uint8*&p){auto v=Read16BE(p);p+=2;return v;}
inline uint32 Read24refBE(uint8*&p){auto v=Read24BE(p);p+=3;return v;}
inline uint32 Read32refBE(uint8*&p){auto v=Read32BE(p);p+=4;return v;}
inline uint64 Read64refBE(uint8*&p){auto v=Read64BE(p);p+=8;return v;}
inline void Write16ref(uint8*&p, uint16 v){Write16(p,v);p+=2;}
inline void Write24ref(uint8*&p, uint32 v){Write24(p,v);p+=3;}
inline void Write32ref(uint8*&p, uint32 v){Write32(p,v);p+=4;}
inline void Write64ref(uint8*&p, uint64 v){Write64(p,v);p+=8;}
inline void Write16refBE(uint8*&p, uint16 v){Write16BE(p,v);p+=2;}
inline void Write24refBE(uint8*&p, uint32 v){Write24BE(p,v);p+=3;}
inline void Write32refBE(uint8*&p, uint32 v){Write32BE(p,v);p+=4;}
inline void Write64refBE(uint8*&p, uint64 v){Write64BE(p,v);p+=8;}
inline void WriteRef(uint8*&p, uint16 v){Write(p,v);p+=2;}
inline void WriteRef(uint8*&p, uint32 v){Write(p,v);p+=4;}
inline void WriteRef(uint8*&p, uint64 v){Write(p,v);p+=8;}
inline void WriteRefBE(uint8*&p, uint16 v){WriteBE(p,v);p+=2;}
inline void WriteRefBE(uint8*&p, uint32 v){WriteBE(p,v);p+=4;}
inline void WriteRefBE(uint8*&p, uint64 v){WriteBE(p,v);p+=8;}
////////////////////////////////////////////////////////////////////////////
inline uint8 Read8(uint8*&p){return *p;}
inline uint8 Read8ref(uint8*&p){return *p++;}
inline void Write8(uint8*&p, uint8 v){*p=v;}
inline void Write8ref(uint8*&p, uint8 v){*p++=v;}
inline void Write(uint8*&p, uint8 v){*p=v;}
inline void Writeref(uint8*&p, uint8 v){*p++=v;}
///////////////////////////////////////////////////////////////////////////
inline uint32 U32Read8 (const uint8*p){return *p;}
inline uint32 U32Read16(const uint8*p){return Read16(p);}
inline uint32 U32Read24(const uint8*p){return Read24(p);}
inline uint32 U32Read16BE(const uint8*p){return Read16BE(p);}
inline uint32 U32Read24BE(const uint8*p){return Read24BE(p);}

inline void U32Write8(uint8*p, uint32 v){*p=v;}
inline void U32Write16(uint8*p, uint32 v){Write16(p,v);}
inline void U32Write24(uint8*p, uint32 v){Write24(p,v);}
inline void U32Write16BE(uint8*p, uint32 v){Write16BE(p,v);}
inline void U32Write24BE(uint8*p, uint32 v){Write24BE(p,v);}
///
inline uint32 U32Read8ref (uint8*&p){return *p++;}
inline uint32 U32Read16ref(uint8*&p){return Read16ref(p);}
inline uint32 U32Read24ref(uint8*&p){return Read24ref(p);}
inline uint32 U32Read16refBE(uint8*&p){return Read16refBE(p);}
inline uint32 U32Read24refBE(uint8*&p){return Read24refBE(p);}

inline void U32Write8ref(uint8*&p, uint32 v){*p++=v;}
inline void U32Write16ref(uint8*&p, uint32 v){Write16ref(p,v);}
inline void U32Write24ref(uint8*&p, uint32 v){Write24ref(p,v);}
inline void U32Write16refBE(uint8*&p, uint32 v){Write16refBE(p,v);}
inline void U32Write24refBE(uint8*&p, uint32 v){Write24refBE(p,v);}
/////////////////////////////////////////////////////////////////
}
