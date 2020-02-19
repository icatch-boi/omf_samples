//
// Created by jg.wang on 2019/11/27.
//

#pragma once

#include <cstring>
#include <iostream>
#include "_dbg_color.h"
namespace omf {
	namespace dbg {
		extern std::ostream *pcout;
		extern void dbgDump0(void* ptr0,int size);
	}
}
///
#define CVAL(v) #v<<'='<<v
#define POSTION strrchr(__FILE__,'/')<<'/'<<__LINE__<<':'<<__FUNCTION__<<"()"
///
#define dbgEntryNormal(s) *omf::dbg::pcout<<s
///
#define dbgEntrySky(s) dbgEntryNormal(DBGSky(s))
#define dbgEntryBlue(s) dbgEntryNormal(DBGBlue(s))
#define dbgEntryGreen(s) dbgEntryNormal(DBGGreen(s))
#define dbgEntryPink(s) dbgEntryNormal(DBGPink(s))
#define dbgEntryRed(s) dbgEntryNormal(DBGRed(s))
#define dbgEntryYellow(s) dbgEntryNormal(DBGYellow(s))
///
#define dbgEntryNote(s) dbgEntryGreen(s)
#define dbgEntryTest(s) dbgEntryNormal(s)
#define dbgEntryErr(s) dbgEntryRed(s)
///
#define dbgDump(ptr,len) omf::dbg::dbgDump0(ptr,len)
///
#define dbgTestDump(ptr,len) dbgDump(ptr,len)
#define dbgTestSL(s) dbgEntryTest(s<<std::endl)
#define dbgTestVL(s) dbgTestSL(CVAL(s))
#define dbgTestPL(s) dbgTestSL(POSTION)
#define dbgTestPSL(s) dbgTestSL(POSTION<<'#'<<s)
#define dbgTestPVL(s) dbgTestPSL(CVAL(s))
///
#define dbgNoteDump(ptr,len) dbgDump(ptr,len)
#define dbgNoteSL(s) dbgEntryNote(s<<std::endl)
#define dbgNoteVL(s) dbgNoteSL(CVAL(s))
#define dbgNotePL(s) dbgNoteSL(POSTION)
#define dbgNotePSL(s) dbgNoteSL(POSTION<<'#'<<s)
#define dbgNotePVL(s) dbgNotePSL(CVAL(s))
///
#define dbgErrDump(ptr,len) dbgDump(ptr,len)
#define dbgErrSL(s) dbgEntryErr(s<<std::endl)
#define dbgErrVL(s) dbgErrSL(CVAL(s))
#define dbgErrPL(s) dbgErrSL(POSTION)
#define dbgErrPSL(s) dbgErrSL(POSTION<<'#'<<s)
#define dbgErrPVL(s) dbgErrPSL(CVAL(s))
///
#define returnIfErrC(v,c) 		if(c){dbgErrPSL(#c);return v;}
#define returnIfErrC0(c)		if(c){dbgErrPSL(#c);return;}
#define returnIfErrCS(v,c,s)	if(c){dbgErrPSL(#c<<','<<s);return v;}
#define returnIfErrCS0(c,s)		if(c){dbgErrPSL(#c<<','<<s);return;}
///
#define returnIfTestC(v,c) 		if(c){dbgTestPSL(#c);return v;}
#define returnIfTestC0(c)		if(c){dbgTestPSL(#c);return;}
#define returnIfTestCS(v,c,s)	if(c){dbgTestPSL(#c<<','<<s);return v;}
#define returnIfTestCS0(c,s)	if(c){dbgTestPSL(#c<<','<<s);return;}
///
#define returnIfNoteC(v,c) 		if(c){dbgNotePSL(#c);return v;}
#define returnIfNoteC0(c)		if(c){dbgNotePSL(#c);return;}
#define returnIfNoteCS(v,c,s)	if(c){dbgNotePSL(#c<<','<<s);return v;}
#define returnIfNoteCS0(c,s)	if(c){dbgNotePSL(#c<<','<<s);return;}
