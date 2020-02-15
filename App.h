#pragma once

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
}
