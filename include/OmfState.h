//
// Created by jg.wang on 2020/4/13.
//
#pragma once

namespace omf {
	namespace api {
		class OmfState {
		public:
			enum class State:int{
				null=1,
				ready=2,
				play=4,
			};
		public:
			/**
			 * change up state, ready/play. \n
			 * 	\li	ready:\n
			 * 	\li	play:\n
			 * @return true/false
			 */
			virtual bool ChangeUp(State)=0;
			/**
			 * change down state, ready/null. \n
			 * 	\li	ready:\n
			 * 	\li	play:\n
			 * @return
			 */
			virtual bool ChangeDown(State)=0;
			/**
			 * change state, up or down, null/ready/plsy. \n
			 * 	\li	null:\n
			 * 	\li	ready:\n
			 * 	\li	play:\n
			 * @return
			 */
			virtual bool ChangeState(State)=0;
			/**
			 * get current state.
			 * @return state:\n
			 * 	\li null:\n
			 * 	\li ready:\n
			 * 	\li play:\n
			 */
			virtual State CurrentState()const=0;
		} ;

		using State = OmfState::State;
	}
}
