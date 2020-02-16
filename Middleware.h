#pragma once

#include <functional>
#include <boost/log/trivial.hpp>

#include "redux.h"
#include "State.h"
#include "Action.h"

namespace App {

	using ThunkPayload = std::function<void(const redux::Dispatch, const redux::GetState<State>)>;

	const redux::MiddlewareDispatchTransform<State>& loggingMiddleware = [](const redux::Middleware<State>& middleware) {
		return [middleware](const redux::Dispatch& dispatch) {
			return [dispatch, middleware](const redux::Action<> action) {
				BOOST_LOG_TRIVIAL(info) << "before dispath action " << toString(action.type().as<ActionType>()) << " state:" << middleware.getState()().toString() << std::endl;
				const redux::Action<>& next = dispatch(action);
				BOOST_LOG_TRIVIAL(info) << "after dispatch action " << toString(next.type().as<ActionType>()) << " state:" << middleware.getState()().toString() << std::endl;
				return next;
			};
		};
	};

	const redux::MiddlewareDispatchTransform<State>& thunkMiddleware = [](const redux::Middleware<State>& middleware) {
		return [middleware](const redux::Dispatch& dispatch) {
			return [dispatch, middleware](const redux::Action<>& action) {
				ActionType type = action.type().as<ActionType>();
				if (type == ActionType::thunk) {
					ThunkPayload thunkPayload = action.payload().as<ThunkPayload>();
					thunkPayload(dispatch, middleware.getState());
					return action;
				}
				return dispatch(action);
			};
		};
	};

}