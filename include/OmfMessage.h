//
// Created by jg.wang on 2020/4/13.
//

#pragma once
#include <string>
#include <functional>
namespace omf {
	namespace api {
		class OmfMessage {
		public:
			using FuncMessage = std::function<bool(const char *)>;
		public:
			/**
			 * register a callback to process the current streaming message.
			 * @param func the callback funnction
			 */
			virtual void RegisterMessageCallback(const FuncMessage &func)=0;

			/**
			 * send a message to current streaming.
			 * @param msg the message script string.
			 * 		eg.. type=message,name=xxx,...
			 * @return true/false
			 */
			virtual bool SendMessage(const char *msg)=0;
		} ;

		using FuncMessage = OmfMessage::FuncMessage;
	}
}
