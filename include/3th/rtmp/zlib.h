//
// Created by wang.zhou on 2018/7/26.
//

#ifndef STREAM_ZLIB_H
#define STREAM_ZLIB_H

#define Z_NO_FLUSH      0


typedef struct z_stream_s {
	const unsigned long* next_in;     /* next input byte */
	unsigned int     avail_in;  /* number of bytes available at next_in */
	unsigned long    total_in;  /* total number of input bytes read so far */

	const unsigned long*  next_out; /* next output byte will go here */
	unsigned int     avail_out; /* remaining free space at next_out */
	unsigned long    total_out; /* total number of bytes output so far */

	const char *msg;  /* last error message, NULL if no error */
	/*struct internal_state FAR *state;*/ /* not visible by applications */

	/*alloc_func zalloc;  /* used to allocate the internal state */
	/*free_func  zfree; */  /* used to free the internal state */
	void*     opaque;  /* private data object passed to zalloc and zfree */

	int     data_type;  /* best guess about the data type: binary or text
                           for deflate, or the decoding state for inflate */
	unsigned long   adler;      /* Adler-32 or CRC-32 value of the uncompressed data */
	unsigned long   reserved;   /* reserved for future use */
} z_stream;


#endif //STREAM_ZLIB_H

int inflateInit(z_stream* stream){
	printf("inflateInit...\n");
	return 0;
}

int inflateEnd(z_stream* stream){
	printf("inflateEnd...\n");
	return 0;
}

int inflate(z_stream* stream,int flush){
	printf("inflate...\n");
	return 0;
}