//
// Created by jg.wang on 2019/7/4.
//
#pragma once

#include <string>
namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				typedef struct AudioMediaInfo{
					AudioMediaInfo(int r=0,int ch=0):rate(r),channels(ch){}
					int rate=0;///< the sample rate.
					int channels=0;///< the channels.
					std::string codec;
					std::string media;
				} AudioMediaInfo;

				typedef struct VideoMediaInfo{
					VideoMediaInfo(int w=0,int h=0):width(w),height(h){}
					int width=0;///< the width.
					int height=0;///< the height.
					std::string codec;
					std::string media;
				} VideoMediaInfo;

				/**
				 * YUV Media info.
				 */
				typedef struct YuvMediaInfo:public VideoMediaInfo {
					YuvMediaInfo(int w=0,int h=0):VideoMediaInfo(w,h){}
					//int width;///< the width of image.
					//int height;///< the height of image.
					std::string format;///< the format of yuv frame. \n such as "422NV16","420V50"
					bool isInterlaced=false; ///< is interlaced or not.
					int frameSize=0; ///< the yuv frame byte size.
					int frameRate=0; ///< the yuv frame rate.
				} YuvMediaInfo;

				/**
				 * H264 I Frame Structure.
				 */
				typedef struct {
					int spsOffset=0;///< the SPS NAL offset
					int spsLenght=0;///< the SPS NAL length.
					int ppsOffset=0;///< the PPS NAL offset
					int ppsLenght=0;///< the PPS NAL length.
					int dataOffset=0;///< the data NAL offset
				} i_frame_t;
				/**
				 * H264 P Frame Structure.
				 */
				typedef struct {
					int dataOffset=0;///< the data NAL offset.
				} p_frame_t;
				/**
				 * H264 Media info.
				 */
				typedef struct :public VideoMediaInfo{
					//int width;///< the width of image.
					//int height;///< the height of image.
					int gop=0;///< the GOP number.
					std::string gopType;///< the frame sequence model.\n such as "IIII","IPPP","IBBP"
					void *sps=0;///< the SPS data pointer.
					int spsLength=0;///< the SPS data length.
					void *pps=0;///< the PPS data pointer.
					int ppsLength=0;///< the PPS data length.
					i_frame_t iframe;///< the I Frame structure.
					p_frame_t pframe;///< the P Frame structure.
					int extraSize=0; ///< the extra data size.
					char *extraData=0;///< the extra data. eg. 00 00 00 01 68 xx ... 00 00 00 67 xx ...
					const char* pack=0;/// the h264 frame package format(avc,bitstream)
					bool (*cbJumpToData)(unsigned char*&data,int& size,const char*pack);
					bool (*cbJumpToDataBs)(unsigned char*&data,int& size);
					bool (*cbJumpToDataAvc)(unsigned char*&data,int& size);
				} H264MediaInfo;

				typedef struct :public VideoMediaInfo{
					//int width;///< the width of image.
					//int height;///< the height of image.
				} JpegMediaInfo;

				/**
				 * PCM Media info.
				 */
				typedef struct PcmMediaInfo:public AudioMediaInfo{
					PcmMediaInfo(int r=0,int ch=0):AudioMediaInfo(r,ch){}
					//int rate;///< the sample rate.
					//int channels;///< the channels.
					bool isSigned=true;///< is signed.
					int bitWidth=16;///< the bit number per value.
					int byteWidth()const{return (bitWidth+7)>>3;}///< the byte width per value.
					int bytePerSample()const{return byteWidth()*channels;}///< byte per sample.
				} PcmMediaInfo;

				/**
				 * AAC Media info.
				 */
				typedef struct :public AudioMediaInfo{
					//int rate;///< the sample rate.
					//int channels;///< the channles.
					bool hasADTS=true;///< aac frame has adts header or not.
					int adtsLength=7;///< the length adts header .
					int profile=0;
					int version=0;
					int rateidx=0;
					int bitrate=0;
					int extraSize=0; ///< the extra data size.
					char *extraData=0;///< the extra data. eg.
				} AacMediaInfo;


			}
		}
	}
}