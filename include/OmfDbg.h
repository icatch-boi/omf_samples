//
// Created by jg.wang on 2019/11/27.
//

#pragma once

#include <cstring>
#include <iostream>
#include "_dbg_color.h"
#include "_dump.h"
#include "_hex.h"
#include "_dbg_api_def.h"
#include "_dbg_cfg_switch.h"
#include "_dbg_cfg_base.h"
#include "_dbg_cout_std.h"
#include "_dbg_cfg_cout.h"
extern const char* dbgGetFileName(const char*path);
///
#define CVAL(v) #v<<'='<<v
#define ENDL() std::endl
#define POSTION dbgGetFileName(__FILE__)<<'/'<<__LINE__<<':'<<__FUNCTION__<<"()"
///
#define dbgTestS(s) dbgEntryTest(s)
///
#define dbgTestSL(s) dbgTestS(s<<ENDL())
#define dbgTestVL(s) dbgTestSL(CVAL(s))
#define dbgTestPL(s) dbgTestSL(POSTION)
#define dbgTestPS(s) dbgTestS(POSTION<<'#'<<s)
#define dbgTestPSL(s) dbgTestSL(POSTION<<'#'<<s)
#define dbgTestPVL(s) dbgTestPSL(CVAL(s))
#define dbgTestD(ptr,len) dbgTestS(dbgEntryDump(ptr,len))
#define dbgTestDL(ptr,len) dbgTestSL(dbgEntryDump(ptr,len))
#define dbgTestPD(ptr,len) dbgTestPS(dbgEntryDump(ptr,len))
#define dbgTestPDL(ptr,len) dbgTestPSL(dbgEntryDump(ptr,len))
///
#define dbgNoteS(s) dbgEntryNote(s)
///
#define dbgNoteSL(s) dbgNoteS(s<<ENDL())
#define dbgNoteVL(s) dbgNoteSL(CVAL(s))
#define dbgNotePL(s) dbgNoteSL(POSTION)
#define dbgNotePS(s) dbgNoteS(POSTION<<'#'<<s)
#define dbgNotePSL(s) dbgNoteSL(POSTION<<'#'<<s)
#define dbgNotePVL(s) dbgNotePSL(CVAL(s))
#define dbgNoteD(ptr,len) 	dbgNoteS(dbgEntryDump(ptr,len))
#define dbgNoteDL(ptr,len) 	dbgNoteSL(dbgEntryDump(ptr,len))
#define dbgNotePD(ptr,len) 	dbgNotePS(dbgEntryDump(ptr,len))
#define dbgNotePDL(ptr,len) dbgNotePSL(dbgEntryDump(ptr,len))
///
#define dbgErrS(s) dbgEntryErr(s)
///
#define dbgErrSL(s) dbgErrS(s<<ENDL())
#define dbgErrVL(s) dbgErrSL(CVAL(s))
#define dbgErrPL(s) dbgErrSL(POSTION)
#define dbgErrPS(s) dbgErrS(POSTION<<'#'<<s)
#define dbgErrPSL(s) dbgErrSL(POSTION<<'#'<<s)
#define dbgErrPVL(s) dbgErrPSL(CVAL(s))
#define dbgErrD(ptr,len) 	dbgErrS(dbgEntryDump(ptr,len))
#define dbgErrDL(ptr,len) 	dbgErrSL(dbgEntryDump(ptr,len))
#define dbgErrPD(ptr,len) 	dbgErrPS(dbgEntryDump(ptr,len))
#define dbgErrPDL(ptr,len) 	dbgErrPSL(dbgEntryDump(ptr,len))
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
