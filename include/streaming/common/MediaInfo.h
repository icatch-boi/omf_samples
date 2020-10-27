//
// Created by jg.wang on 2019/7/4.
//
#pragma once

#include <string>
namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				/**
				 * YUV Media info.
				 */
				typedef struct {
					int width;///< the width of image.
					int height;///< the height of image.
					std::string format;///< the format of yuv frame. \n such as "422NV16","420V50"
					bool isInterlaced; ///< is interlaced or not.
					int frameSize; ///< the yuv frame byte size.
					int frameRate; ///< the yuv frame rate.
				} YuvMediaInfo;

				/**
				 * H264 I Frame Structure.
				 */
				typedef struct {
					int spsOffset;///< the SPS NAL offset
					int spsLenght;///< the SPS NAL length.
					int ppsOffset;///< the PPS NAL offset
					int ppsLenght;///< the PPS NAL length.
					int dataOffset;///< the data NAL offset
				} i_frame_t;
				/**
				 * H264 P Frame Structure.
				 */
				typedef struct {
					int dataOffset;///< the data NAL offset.
				} p_frame_t;
				/**
				 * H264 Media info.
				 */
				typedef struct {
					int width;///< the width of image.
					int height;///< the height of image.
					int gop;///< the GOP number.
					std::string gopType;///< the frame sequence model.\n such as "IIII","IPPP","IBBP"
					void *sps;///< the SPS data pointer.
					int spsLength;///< the SPS data length.
					void *pps;///< the PPS data pointer.
					int ppsLength;///< the PPS data length.
					i_frame_t iframe;///< the I Frame structure.
					p_frame_t pframe;///< the P Frame structure.
					int extraSize; ///< the extra data size.
					char *extraData;///< the extra data. eg. 00 00 00 01 68 xx ... 00 00 00 67 xx ...
					const char* pack;/// the h264 frame package format(avc,bitstream)
					bool (*cbJumpToData)(unsigned char*&data,int& size,const char*pack);
					bool (*cbJumpToDataBs)(unsigned char*&data,int& size);
					bool (*cbJumpToDataAvc)(unsigned char*&data,int& size);
				} H264MediaInfo;

				typedef struct {
					int width;///< the width of image.
					int height;///< the height of image.
				} JpegMediaInfo;

				/**
				 * PCM Media info.
				 */
				typedef struct {
					int rate;///< the sample rate.
					int channels;///< the channels.
					bool isSigned;///< is signed.
					int bitWidth;///< the bit number per value.
					int byteWidth()const{return (bitWidth+7)>>3;}///< the byte width per value.
					int bytePerSample()const{return byteWidth()*channels;}///< byte per sample.
				} PcmMediaInfo;

				/**
				 * AAC Media info.
				 */
				typedef struct {
					int rate;///< the sample rate.
					int channels;///< the channles.
					bool hasADTS;///< aac frame has adts header or not.
					int adtsLength;///< the length adts header .
					int profile;
					int version;
					int rateidx;
					int bitrate;
					int extraSize; ///< the extra data size.
					char *extraData;///< the extra data. eg.
				} AacMediaInfo;

				typedef struct {
					int rate;///< the sample rate.
					int channels;///< the channels.
					std::string codec;
					std::string media;
				} AudioMediaInfo;

				typedef struct {
					int width;///< the width.
					int height;///< the height.
					std::string codec;
					std::string media;
				} VideoMediaInfo;
			}
		}
	}
}