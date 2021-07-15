#ifndef __AAC_ERROR_H__
#define __AAC_ERROR_H__

typedef enum{
	AAC_ERR_DEC_ADTS_Syncword = -10000,
	AAC_ERR_DEC_NULLHeader_NoSupport,

	AAC_ERR_DEC_Book=-20000,
	AAC_ERR_DEC_NoSupportPulseData,
	AAC_ERR_DEC_NoSupportTNS,		
	AAC_ERR_DEC_NoSupportGainControl,
	AAC_ERR_DEC_NoSupportLTP,
	AAC_ERR_DEC_NoSupportPredict,

	AAC_ERR_DEC_CpeID=-25000,
	AAC_ERR_DEC_CpeChannelTag,
	AAC_ERR_DEC_CpeCommonChannel,

	AAC_ERR_ENC_NoSupportPulseData=-50000,
	AAC_ERR_ENC_NoSupportTNS,		
	AAC_ERR_ENC_NoSupportGainControl,
	AAC_ERR_ENC_NoSupportLTP,
	AAC_ERR_ENC_NoSupportPredict,

	AAC_ERR_CODEC_InvalidSampleRate=-80000,
	AAC_ERR_CODEC_NoSupportChn,
	AAC_ERR_CODEC_NoSupportBitPerSample,
	AAC_ERR_CODEC_InputNotEnough,
}AacError_e;
AacError_e AacGetError();
#endif

