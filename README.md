# redux.cpp
Implement redux framework by c++17

# Open Redux.sln debug App.cpp

```
#include <iostream>
#include <vector>
#include "redux.h"

struct State {
	std::string toString() { return "counter: " + std::to_string(_counter); }
	int _counter{ 0 };
};

enum class ActionType {
	increment,
	decrement,
	thunk
};

struct Increment {
	redux::Any payload() const { return _payload; }
	redux::Any type() const { return _type; }
	int _payload;
	ActionType _type = { ActionType::increment };
};

struct Decrement {
	redux::Any payload() const { return _payload; }
	redux::Any type() const { return _type; }
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

std::string toString(ActionType type) {
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

	const redux::Reducer<State>& reducer = [](State state, redux::Action<> action) {

		int multiplier = 1;
		auto type = action.type().as<ActionType>();
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

		auto payload = action.payload().as<int>();
		state._counter += multiplier * payload;
		return state;
	};

	const redux::MiddlewareDispatchTransform<State>& loggingMiddleware = [](redux::Middleware<State> middleware) {
		return [=](const redux::Dispatch& dispatch) {
			return [=](redux::Action<> action) -> redux::Action<> {
				std::cout << "log before dispath action type: " << toString(action.type().as<ActionType>()) << " state:" << middleware.getState()().toString() << std::endl;
				const redux::Action<>& next = dispatch(action);
				std::cout << "log after dispatch action type: " << toString(action.type().as<ActionType>()) << " state:" << middleware.getState()().toString() << std::endl;
				return next;
			};
		};
	};

	const redux::MiddlewareDispatchTransform<State>& thunkMiddleware = [](redux::Middleware<State> middleware) {
		return [=](const redux::Dispatch& dispatch) {
			return [=](redux::Action<> action) -> redux::Action<> {
				auto type = action.type().as<ActionType>();
				if (type == ActionType::thunk) {
					ThunkPayload thunkPayload = action.payload().as<ThunkPayload>();
					thunkPayload(dispatch, middleware.getState());
					return action;
				}
				return dispatch(action);
			};
		};
	};

	auto store = redux::createStore<State>(
		reducer,
		State{},
		{
			thunkMiddleware,
			loggingMiddleware
		}
	);

	std::cout << "init state: " << store.state().toString() << std::endl;

	store.dispatch(Thunk{
		[&](redux::Dispatch dispatch, redux::GetState<State> getState) {
			dispatch(Increment{ 100 });
			dispatch(Increment{ 300 });
		}
	});

	store.dispatch(Increment{1000});

	return 0;
}
```
