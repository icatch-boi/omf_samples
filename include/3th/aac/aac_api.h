#ifndef __AAC_API_H__
#define __AAC_API_H__
#include "aac_param.h"
#include "aac_error.h"

/* AacOpenEncode
 ** open encoder
 * params:
 ** hd			[input] existed hd;if hd==0, return new hd;
 ** sampleRate	[input] sarmple rate
 ** channels		[input] channel number
 ** depth		[input] bit per sample
 ** hasAdts		[input] add adts header or not
 ** bitRate		[input] bit rate 
 ** bandWidth   [input] frequency bandwidth 
 * return:
 ** instance handle
 */
void* AacOpenEncode(void* hd, int sampleRate, int channels, int depth, int hasAdts, int bitrate, int bandWidth);

/* AacOpenDecode
 ** open decoder
 * params:
 ** hd			[input] existed hd;if hd==0, return new hd;
 ** sampleRate	[input] sarmple rate
 ** channels		[input] channel number
 ** depth		[input] bit per sample
 ** hasAdts		[input] add adts header or not
 * return:
 ** instance handle
 */
void* AacOpenDecode(void* hd,int sampleRate,int channels,int depth,int hasAdts);

/* AacReset
 ** reset decoder/encoder 
 */
void AacReset(void* hd);

/* AacClose
 ** close codec
 * params:
 ** isFree: destroy aac inside struct or not
 * return:
 ** void
 */
void AacClose(void* hd,int isFreeMemory);

/* AacEncode
 ** encode pcm stream to aac frame,
 ** and pack the frame to bit stream.
 * params:
 ** *pcmpt:   input pcm data pointer
 ** pcmsize: input pcm data size
 ** *aacpt:  output aac bit stream buf pointer
 ** aacsize:output aac bit stream buf size(max byte size)
 * return:
 ** output aac bit stream buf size used 
 */
int AacEncode(void* hd,uint8* pcmpt,int pcmsize,uint8* aacpt,int aacsize);

/* AacDecode
 ** unpack aac bit stream,
 ** get one frame data,
 ** and decode the frame to pcm stream.
 * params:
 ** *mp3pt:   input aac bit stream buf pointer
 ** mp3size: input aac bit stream buf size(max byte size)
 ** *pcmpt:  output pcm data pointer
 ** pcmsize:output pcm data size
 * return:
 ** output aac bit stream buf size used 
 */
int AacDecode(void* hd,uint8* aacpt,int aacsize,uint8* pcmpt,int pcmsize);

/* AacParamGet
 ** output aac codec param
 ** modify is invalid
 *param: 
 ** *cfg	:output codec AacConfig_t struct copy
 *return:
 ** error code
 */
int AacParamGet(void* hd,AacConfig_t* cfg);

/* AacParamGetPtr
 ** output aac config intern ptr
 *param: 
 ** *hd	
 *return:
 ** AacConfig_t intern ptr
 */
AacConfig_t* AacParamGetPtr(void* hd);

/* AacErrorGet
 *return:
 ** last error code
 */
AacError_e AacErrorGet(void* hd);



/* AacParamSet
 ** set aac codec param
 * params:
 ** selector:	input param id
 ** val:			input param value
 * return:
 ** no define
 */
uint32 AacParamSet(void* hd,AacParam_e selector,uint32 val);
int aacAdtsUpdate(void* hd,uint8* adts,int size);


int AacSampleRateConvert(int sr);
int AacExtraData(char* extra,int profile,int rate,int ch);
 
/* AacInitialize()
** initialize aac codec globle data table
*/
void AacInitialize();

/* AacJion
** Jion two aac frame
* params:
** frame0/size0:	[input]1st aac frame
** frame1/size1:	[input]2nd aac frame
** mixframe/dsize:[input]mix frame temp buffer
** sampleRate	[input]sample rate	,fill 0 when hasadts==1
** channels	[input]channels number	,fill 0 when hasadts==1
** bitnumber: [input]sample bit number,fill 0 when hasadts==1
** bitrate:	[input]mixframe bitrate
return:
mixframe size
*/
int AacJoin(
	uint8* frame0, int size0
	, uint8* frame1, int size1
	, uint8* mixframe, int dsize
	, int sampleRate
	, int channels
	, int bitdepth
	, int hasadts
	, int bitrate	
);

typedef struct{
	uint8* src0; int size0;	// 1st aac
	uint8* mix; int size;	// mix aac frame
	uint8* src1; int size1;	// 2nd aac
}AacJionResult_t;
/* AdtsJoin
** Jion two aac buffer(include adts header)
* params:
** rst:			[output]jion aac result
** src0/size0:	[input]1st source aac
** src1/size1:	[input]2nd source aac 
** dst/dsize:	[input]dst aac buffer
* return:
** error
*/
int AdtsJoin(
	AacJionResult_t* rst
	, uint8* src0, int size0
	, uint8* src1, int size1
	, uint8* dst, int dsize
	, int bitrate
	);



#endif


