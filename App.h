#pragma once

#include <boost/log/trivial.hpp>
#include "redux.h"

namespace App {
	class State {
	public:
		explicit State(int counter = 0) : _counter(counter) {};
		int counter() const { return _counter; };
		std::string toString() const { return "counter: " + std::to_string(_counter); }
	private:
		int _counter;
	};

	template<typename Type, typename Payload>
	class MyAction {
	public:
		explicit MyAction(const Type& type, const Payload& payload) : _type(type), _payload(payload) {};
		Payload payload() const { return _payload; }
		Type type() const { return _type; }
	private:
		Type _type;
		Payload _payload;
	};

	enum class ActionType {
		increment,
		decrement,
		thunk
	};

	const auto increment = [](const State& state, int payload) {
		return State{ state.counter() + payload };
	};

	const auto decrement = [](const State& state, int payload) {
		return State{ state.counter() - payload };
	};

	const std::unordered_map<ActionType, std::function<const State(const State&, int)>> funcMap = {
		{ActionType::increment, increment},
		{ActionType::decrement, decrement}
	};

	const redux::Reducer<State>& reducer = [](const State& state, const redux::Action<>& action) {

		ActionType actionType = action.type().as<ActionType>();

		int payload = action.payload().as<int>();

		auto search = funcMap.find(actionType);

		if (search != funcMap.end())
			return search->second(state, payload);
		else
			return state;
	};

	using ThunkPayload = std::function<void(const redux::Dispatch, const redux::GetState<State>)>;

	const std::string toString(ActionType type) {
		switch (type) {
		case ActionType::increment:
			return "inc";
		case ActionType::decrement:
			return "dec";
		case ActionType::thunk:
			return "thunk";
		default:
			return "";
		}
	}

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
