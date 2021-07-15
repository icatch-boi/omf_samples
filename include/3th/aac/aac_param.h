#ifndef __AAC_PARAM_H__
#define __AAC_PARAM_H__

typedef enum{
	AAC_MPEG4=0,
	AAC_MPEG2=1,
}AacMpegVersion;
typedef enum{
	AAC_OBJ_MAIN=0,
	AAC_OBJ_LS,
	AAC_OBJ_SSR,
	AAC_OBJ_LTP,
}AacProFile;
typedef enum{
	AAC_WIN_SHAPE_SIN=0,
	AAC_WIN_SHAPE_KBD,
}AacWinShape;
typedef struct AacConfig_s{ 
	/* AacMpegVersion-MPEG version, (MPEG4=0,MPEG2) */
	char mpegVersion;

	/* AacProFile-AAC object type (MAIN=0,LS,SSR,LTP) */
	char profile;

	/* AacWinShape-window shape:(sin=0,kbd)*/
	char shape;	

	/* bitrate / channel of AAC file */
	int bitRate;
	int bitRateMax;
	int samplePerFrame;
	int pcmBytePerFrame;

	/* func enable*/
	char funcMS;//mide side
	char funcPD;//pulse data
	char funcTNS;// Use Temporal Noise Shaping 
	char funcGC;//gain control
	char funcPred;//predictor data or long time predict	

	int sampleRate;
	char sampleRateIndex;//
	char channels;
	char width;//byte per ch per sample
	char depth;//bits per ch per sample
	
	char hasAdts;
	char channelsPcm;
	//char plus;
	//char quality;/* Quantizer quality */
	int bandWidth;/* AAC file frequency bandwidth */
	//int lowFreqThrLong,lowFreqThrShort;
	//void* psymodel;	
	unsigned char *adts;//[16];
	int adtsLen;

	unsigned char extra[4];
	int extraLen;
}AacConfig_t;

typedef enum{
	AAC_PARAM_ENC_ChannelsOfAac,
	AAC_PARAM_DEC_ChannelsOfPcm,
	AAC_PARAM_MaxBitRate,
}AacParam_e;
#endif

