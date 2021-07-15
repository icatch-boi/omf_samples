//
// Created by yb.leng on 2020/3/8.
//
#pragma once

typedef struct DiskInfo_s {
	int actDsk;	/* active disk : default 0 (not init yet) */
	int totalSz;	/* disk size (MB): default 0 */
	int freeSz;	/* disk remain free size in (KB) : default 0 */
	int cluSz;	/* disk cluster size(MB) : default 0*/
} DiskInfo_t;
