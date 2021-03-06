# redux.cpp
Implement redux framework by c++17

# Quick Start

```C++
#include "redux.h"
#include "State.h"
#include "Action.h"
#include "Reducer.h"
#include "Middleware.h"

int main() {

	redux::Store<App::State> store = redux::createStore<App::State>(
		App::reducer,
		App::State{},
		{
			App::thunkMiddleware,
			App::loggingMiddleware
		}
	);

	BOOST_LOG_TRIVIAL(info) << "init state: " << store.state().toString() << std::endl;

	store.dispatch(App::MyAction<App::ActionType, App::ThunkPayload> {
		App::ActionType::thunk,
		[](const redux::Dispatch& dispatch, const redux::GetState<App::State>& getState) {
			dispatch(App::MyAction<App::ActionType, int>{App::ActionType::increment, 100 });
			dispatch(App::MyAction<App::ActionType, int>{App::ActionType::decrement, 80});
		}
	});

	store.dispatch(App::MyAction<App::ActionType, int>{App::ActionType::increment, 10});

	BOOST_LOG_TRIVIAL(info) << "end state: " << store.state().toString() << std::endl;

	return 0;
}
```
