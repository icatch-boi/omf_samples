#pragma once

typedef struct _RTPHeader{
	//LITTLE_ENDIAN
	unsigned int csrc_count:4;    /* CSRC count */
	unsigned int extension:1;     /* header extension flag */
	unsigned int padding:1;       /* padding flag */
	unsigned int version:2;       /* protocol version */
	unsigned int payload_type:7;  /* payload type */
	unsigned int marker:1;        /* marker bit */
	//BIG_ENDIAN
	//unsigned int version:2;       /* protocol version */
	//unsigned int padding:1;       /* padding flag */
	//unsigned int extension:1;     /* header extension flag */
	//unsigned int csrc_count:4;    /* CSRC count */
	//unsigned int marker:1;        /* marker bit */
	//unsigned int payload_type:7;  /* payload type */

	unsigned int seq:16;          /* sequence number */
	unsigned int pts:32;    /* pts */
	unsigned int ssrc:32;         /* synchronization source */
	//guint8 csrclist[4];           /* optional CSRC list, 32 bits each */
	unsigned char payhead[24];
	//unsigned char payload1;
} RTPHeader;

typedef struct{
	unsigned int profile1:8;
	unsigned int profile2:8;
	unsigned int ehl1:8;
	unsigned int ehl0:8;
	unsigned index:32;
} RTPExternHeader;

#define RTP_HEADER_VERSION(data)      (((RTPHeader *)(data))->version)
#define RTP_HEADER_PADDING(data)      (((RTPHeader *)(data))->padding)
#define RTP_HEADER_EXTENSION(data)    (((RTPHeader *)(data))->extension)
#define RTP_HEADER_CSRC_COUNT(data)   (((RTPHeader *)(data))->csrc_count)
#define RTP_HEADER_MARKER(data)       (((RTPHeader *)(data))->marker)
#define RTP_HEADER_PAYLOAD_TYPE(data) (((RTPHeader *)(data))->payload_type)
//#define RTP_HEADER_SEQ(data)          (((RTPHeader *)(data))->seq)
//#define RTP_HEADER_TIMESTAMP(data)    (((RTPHeader *)(data))->pts)
//#define RTP_HEADER_SSRC(data)         (((RTPHeader *)(data))->ssrc)
//#define RTP_HEADER_PAYHEAD(data,i)    (((RTPHeader *)(data))->payhead[i])
#define RTP_HEADER_SEQ(data)          (*((uint16*)(data)+1))
#define RTP_HEADER_TIMESTAMP(data)    (*((uint32*)(data)+1))
#define RTP_HEADER_SSRC(data)         (*((uint32*)(data)+2))
#define RTP_HEADER_PAYHEAD_PTR(data)    ((uint8 *)(data)+12)
#define RTP_HEADER_PAYHEAD(data,i)    (*((uint8 *)(data)+12+i))

#define RTP_HEADER_LEN 12
#define RTP_EXTERN_HEADER_LEN sizeof(RTPExternHeader)



