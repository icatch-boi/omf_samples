//
// Created by jg.wang on 2019/8/22.
//
#pragma once
#include <chrono>
#include <functional>
#include <memory>
#include <string>
namespace omf {
	namespace api {
		class frame_t {
		public:
			int index;
			void *data;
			int size;
			bool iskeyframe;
			std::chrono::steady_clock::time_point pts;
			std::function<void()> free;
			void*pkt;
			void*creater;
			unsigned slice;
			const char* codec;
			unsigned flags;
		public:
			/**
			 * @brief run the free callback when deconstruct.
			 */
			~frame_t() { if (free)free(); }

			/**
			 * @brief reset the free callback to nullptr.
			 */
			void reset() { free = nullptr; }
		public:
			using sptr=std::shared_ptr<frame_t>;
		} ;
		///
		class OmfFrame{
		public:
			using frame_t = omf::api::frame_t;
			using Frame=std::shared_ptr<frame_t>;
			using FuncFrame = std::function<bool(Frame)>;
			using FuncFrameRef = std::function<bool(Frame&)>;
			using FuncFrameReturn = std::function<Frame()>;
		};
		///
		using FuncFrame = OmfFrame::FuncFrame;
		using FuncFrameRef = OmfFrame::FuncFrameRef;
		using FuncFrameReturn = OmfFrame::FuncFrameReturn;
		using Frame = OmfFrame::Frame;
	}
}
