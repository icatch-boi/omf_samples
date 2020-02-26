#pragma once
////////////////////////////////////////////////////////////////
///
#define dbgEntryBaseChn(o,s)	o<<s
#define dbgEntryBase0(s)		dbgEntryChn(DBG_COUT_CHN_0,s)
#define dbgEntryBase1(s)		dbgEntryChn(DBG_COUT_CHN_1,s)
#define dbgEntryBase2(s)		dbgEntryChn(DBG_COUT_CHN_2,s)
#define dbgEntryBase3(s)		dbgEntryChn(DBG_COUT_CHN_3,s)
#define dbgEntryBase4(s)		dbgEntryChn(DBG_COUT_CHN_4,s)
#define dbgEntryBase5(s)		dbgEntryChn(DBG_COUT_CHN_5,s)
#define dbgEntryBase6(s)		dbgEntryChn(DBG_COUT_CHN_6,s)
#define dbgEntryBase7(s)		dbgEntryChn(DBG_COUT_CHN_7,s)

////////////////////////////////////////////////////////////////
#define dbgEntryBaseColor(o,s)	o<<s
#define dbgEntryBaseRed(s)		dbgEntryColor(DBG_COUT_RED	,DBGRed(s))
#define dbgEntryBaseGreen(s)	dbgEntryColor(DBG_COUT_GREEN,DBGGreen(s))
#define dbgEntryBaseYellow(s)	dbgEntryColor(DBG_COUT_YELLOW,DBGYellow(s))
#define dbgEntryBaseBlue(s)		dbgEntryColor(DBG_COUT_BLUE	,DBGBlue(s))
#define dbgEntryBasePink(s)		dbgEntryColor(DBG_COUT_PINK	,DBGPink(s))
#define dbgEntryBaseSky(s)		dbgEntryColor(DBG_COUT_SKY	,DBGSky(s))

////////////////////////////////////////////////////////////////
#define dbgEntryBaseSpec(o,s)	o<<s
#define dbgEntryBaseErr(s)		dbgEntrySpec(DBG_COUT_ERR,DBGRed(s)) //red
#define dbgEntryBaseLog(s)		dbgEntrySpec(DBG_COUT_LOG,DBGYellow(s))//yellow
#define dbgEntryBaseInfo(s)		dbgEntrySpec(DBG_COUT_INFO,s)
#define dbgEntryBaseTest(s)		dbgEntrySpec(DBG_COUT_TEST,s)
#define dbgEntryBaseNote(s)		dbgEntrySpec(DBG_COUT_NOTE,DBGGreen(s))

////////////////////////////////////////////////////////////////
#define dbgEntryBaseLv(o,s)		o<<s
#define dbgEntryBaseLv0(s)		dbgEntryLv(DBG_COUT_LV_0,s)
#define dbgEntryBaseLv1(s)		dbgEntryLv(DBG_COUT_LV_1,s)
#define dbgEntryBaseLv2(s)		dbgEntryLv(DBG_COUT_LV_2,s)
#define dbgEntryBaseLv3(s)		dbgEntryLv(DBG_COUT_LV_3,s)
#define dbgEntryBaseLv4(s)		dbgEntryLv(DBG_COUT_LV_4,s)
#define dbgEntryBaseLv5(s)		dbgEntryLv(DBG_COUT_LV_5,s)
#define dbgEntryBaseLv6(s)		dbgEntryLv(DBG_COUT_LV_6,s)
#define dbgEntryBaseLv7(s)		dbgEntryLv(DBG_COUT_LV_7,s)
////////////////////////////////////////////////////////////////
#define dbgEntryBaseOut(o,v)		o<<v

////////////////////////////////////////////////////////////////
#define dbgEntryBaseDump(ptr,len) omf::Dump{((unsigned long long)(ptr)<<32)|(len)}//__dbgDump(chn, ptr,len)

