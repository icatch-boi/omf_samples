//
// Created by jg.wang on 2019/8/22.
//
#pragma once
#ifndef _OMF_API_FRAME_H__
#define _OMF_API_FRAME_H__
#include <chrono>
#include <functional>
#include <memory>
#include <string>
namespace omf {
	namespace api {
		namespace streaming {
			namespace common {
				typedef struct frame_t {
					int index;
					void *data;
					int size;
					bool iskeyframe;
					std::chrono::steady_clock::time_point pts;
					std::function<void()> free;

					/**
					 * @brief run the free callback when deconstruct.
					 */
					~frame_t() { if (free)free(); }

					/**
					 * @brief reset the free callback to nullptr.
					 */
					void reset() { free = nullptr; }
				} frame_t;

				enum class State{
					null=1,
					ready=2,
					play=4,
				};

				using FuncFrame = std::function<bool(std::shared_ptr<frame_t>&)>;
				using FuncMessage = std::function<bool(const char *)>;
			}
		}
	}
}
#endif
