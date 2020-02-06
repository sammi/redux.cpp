# redux.cpp
Implement redux framework by c++17

# Quick Start

```
#include <iostream>
#include <vector>
#include "redux.h"

struct State {
	std::string toString() const { return "counter: " + std::to_string(_counter); }
	int _counter{ 0 };
};

enum class ActionType {
	increment,
	decrement,
	thunk
};

struct Increment {
	int payload() const { return _payload; }
	ActionType type() const { return _type; }
	int _payload;
	ActionType _type = { ActionType::increment };
};

struct Decrement {
	int payload() const { return _payload; }
	ActionType type() const { return _type; }
	int _payload;
	ActionType _type = { ActionType::decrement };
};

using ThunkPayload = std::function<void(const redux::Dispatch, const redux::GetState<State>)>;

struct Thunk {
	ThunkPayload payload() const { return _payload; }
	ActionType type() const { return _type; }
	ThunkPayload _payload;
	ActionType _type{ ActionType::thunk };
};

const std::string toString(ActionType type) {
	switch (type) {
	case ActionType::increment:
		return "inc";
	case ActionType::decrement:
		return "dec";
	case ActionType::thunk:
		return "thunk";
	}
}

int main() {

	const redux::Reducer<State>& reducer = [](const State& state, const redux::Action<>& action) {

		int multiplier = 1;
		ActionType type = action.type().as<ActionType>();
		switch (type) {
		case ActionType::decrement:
			multiplier = -1;
			break;
		case ActionType::increment:
			multiplier = 1;
			break;
		default:
			break;
		}

		int payload = action.payload().as<int>();

		State newState{ state };
		newState._counter += multiplier * payload;
		return newState;
	};

	const redux::MiddlewareDispatchTransform<State>& loggingMiddleware = [](const redux::Middleware<State>& middleware) -> const redux::DispatchTransform {
		return [=](const redux::Dispatch& dispatch) -> const redux::Dispatch {
			return [=](const redux::Action<> action) -> const redux::Action<> {
				std::cout << "log before dispath action type: " << toString(action.type().as<ActionType>()) << " state:" << middleware.getState()().toString() << std::endl;
				const redux::Action<>& next = dispatch(action);
				std::cout << "log after dispatch action type: " << toString(next.type().as<ActionType>()) << " state:" << middleware.getState()().toString() << std::endl;
				return next;
			};
		};
	};

	const redux::MiddlewareDispatchTransform<State>& thunkMiddleware = [](const redux::Middleware<State>& middleware) -> const redux::DispatchTransform {
		return [=](const redux::Dispatch& dispatch) -> redux::Dispatch {
			return [=](const redux::Action<>& action) -> redux::Action<> {
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

	redux::Store<State> store = redux::createStore<State>(
		reducer,
		State{},
		{
			thunkMiddleware,
			loggingMiddleware
		}
	);

	std::cout << "init state: " << store.state().toString() << std::endl;

	store.dispatch(Thunk{
		[=](const redux::Dispatch& dispatch, const redux::GetState<State>& getState) -> void {
			std::cout << " state 1 : " << getState().toString() << std::endl;
			dispatch(Increment{ 100 });
			std::cout << " state 2 : " << getState().toString() << std::endl;
			dispatch(Increment{ 300 });
			std::cout << " state 3 : " << getState().toString() << std::endl;
		}
	});

	store.dispatch(Increment{1000});

	return 0;
}
```
