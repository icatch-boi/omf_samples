#ifndef __AAC_ADTS_H__
#define __AAC_ADTS_H__
//#include "_type.h"
//#define uint16 unsigned short
//#define uint8 unsigned char
typedef struct AacAdts_t{
	unsigned short syncword;
	unsigned char mpgeversion;/* ID == 0 for MPEG4 AAC, 1 for MPEG2 AAC */
	unsigned char layer;
	unsigned char absent;
	unsigned char profile;
	unsigned char samplingrate;
	unsigned char privatebit;
	unsigned char numchnnel;
	unsigned char original_copy;
	unsigned char home;

	unsigned char id_bit;
	unsigned char id_start;
	unsigned short usedbytes;
	unsigned short bufferfullness;
	unsigned char rawdatablocks;
}AacAdts_t;

int aacAdtsParse(unsigned char* buf,int len,AacAdts_t *adts);
int aacAdtsCreate(unsigned char* buf,int len,AacAdts_t *adts);

#endif
