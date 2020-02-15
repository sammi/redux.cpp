# redux.cpp
Implement redux framework by c++17

# Quick Start

```C++
#include <boost/log/trivial.hpp>
#include "App.h"

int main() {

	const redux::Reducer<App::State>& reducer = [](const App::State& state, const redux::Action<>& action) {

		int multiplier = 1;
		App::ActionType type = action.type().as<App::ActionType>();
		switch (type) {
		case App::ActionType::decrement:
			multiplier = -1;
			break;
		case App::ActionType::increment:
			multiplier = 1;
			break;
		default:
			break;
		}

		int payload = action.payload().as<int>();

		return App::State{ state.counter() + multiplier * payload };
	};

	const redux::MiddlewareDispatchTransform<App::State>& loggingMiddleware = [](const redux::Middleware<App::State>& middleware) {
		return [middleware](const redux::Dispatch& dispatch) {
			return [dispatch, middleware](const redux::Action<> action) {
				BOOST_LOG_TRIVIAL(info) << "log before dispath action type: " << App::toString(action.type().as<App::ActionType>()) << " state:" << middleware.getState()().toString() << std::endl;
				const redux::Action<>& next = dispatch(action);
				BOOST_LOG_TRIVIAL(info) << "log after dispatch action type: " << App::toString(next.type().as<App::ActionType>()) << " state:" << middleware.getState()().toString() << std::endl;
				return next;
			};
		};
	};

	const redux::MiddlewareDispatchTransform<App::State>& thunkMiddleware = [](const redux::Middleware<App::State>& middleware) {
		return [middleware](const redux::Dispatch& dispatch) {
			return [dispatch, middleware](const redux::Action<>& action) {
				App::ActionType type = action.type().as<App::ActionType>();
				if (type == App::ActionType::thunk) {
					App::ThunkPayload thunkPayload = action.payload().as<App::ThunkPayload>();
					thunkPayload(dispatch, middleware.getState());
					return action;
				}
				return dispatch(action);
			};
		};
	};

	redux::Store<App::State> store = redux::createStore<App::State>(
		reducer,
		App::State{},
		{
			thunkMiddleware,
			loggingMiddleware
		}
	);

	BOOST_LOG_TRIVIAL(info) << "init state: " << store.state().toString() << std::endl;

	store.dispatch(App::MyAction<App::ActionType, App::ThunkPayload> {
		App::ActionType::thunk,
		[](const redux::Dispatch& dispatch, const redux::GetState<App::State>& getState) {
			BOOST_LOG_TRIVIAL(info) << " state 1 : " << getState().toString() << std::endl;
			dispatch(App::MyAction<App::ActionType, int>{App::ActionType::increment, 100 });
			BOOST_LOG_TRIVIAL(info) << " state 2 : " << getState().toString() << std::endl;
			dispatch(App::MyAction<App::ActionType, int>{App::ActionType::decrement, 367 });
			BOOST_LOG_TRIVIAL(info) << " state 3 : " << getState().toString() << std::endl;
		}
	});

	store.dispatch(App::MyAction<App::ActionType, int>{App::ActionType::increment, 1000});

	return 0;
}
```
