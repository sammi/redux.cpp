#include <vector>
#include <boost/log/trivial.hpp>

#include "redux.h"

class State {
public:
	explicit State(int counter=0) : _counter(counter) {};
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

		return State{ state.counter() + multiplier * payload };
	};

	const redux::MiddlewareDispatchTransform<State>& loggingMiddleware = [](const redux::Middleware<State>& middleware) {
		return [middleware](const redux::Dispatch& dispatch) {
			return [dispatch, middleware](const redux::Action<> action) {
				BOOST_LOG_TRIVIAL(info) << "log before dispath action type: " << toString(action.type().as<ActionType>()) << " state:" << middleware.getState()().toString() << std::endl;
				const redux::Action<>& next = dispatch(action);
				BOOST_LOG_TRIVIAL(info) << "log after dispatch action type: " << toString(next.type().as<ActionType>()) << " state:" << middleware.getState()().toString() << std::endl;
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

	redux::Store<State> store = redux::createStore<State>(
		reducer,
		State{},
		{
			thunkMiddleware,
			loggingMiddleware
		}
	);

	BOOST_LOG_TRIVIAL(info) << "init state: " << store.state().toString() << std::endl;

	store.dispatch(MyAction<ActionType, ThunkPayload> {
		ActionType::thunk,
		[](const redux::Dispatch& dispatch, const redux::GetState<State>& getState) {
			BOOST_LOG_TRIVIAL(info) << " state 1 : " << getState().toString() << std::endl;
			dispatch(MyAction<ActionType, int>{ActionType::increment, 100 });
			BOOST_LOG_TRIVIAL(info) << " state 2 : " << getState().toString() << std::endl;
			dispatch(MyAction<ActionType, int>{ActionType::decrement, 367 });
			BOOST_LOG_TRIVIAL(info) << " state 3 : " << getState().toString() << std::endl;
		}
	});

	store.dispatch(MyAction<ActionType, int>{ActionType::increment, 1000});

	return 0;
}
