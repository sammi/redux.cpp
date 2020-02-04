#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <functional>
#include <optional>
#include <unordered_map>
#include <initializer_list>
#include <numeric>
#include <vector>

namespace redux {

	class Any {
	public:
		Any() = default;

		Any(const Any& other) {
			if (other._p) _p = std::unique_ptr<concept>(other._p->copy());
		}

		template <typename T>
		Any(const T& t) : _p(new concrete<T>(t)) {}

		template <typename T>
		Any& operator=(T&& t) {
			_p.reset(new concrete<T>(std::forward<T>(t)));
			return *this;
		}

		Any& operator=(Any other) {
			_p = std::move(other._p);
			return *this;
		}

		Any& operator=(Any&& other) noexcept {
			_p = std::move(other._p);
			return *this;
		}

		template <typename T>
		T& as() {
			return static_cast<concrete<T>*>(_p.get())->_t;
		}

		operator bool() const { return (_p) ? true : false; }

	private:
		struct concept {
			virtual ~concept() = default;
			virtual concept* copy() const = 0;
		};

		template <typename T>
		struct concrete : concept {
			concrete(T const& t) : _t(t) {}

			concept* copy() const override { return new concrete<T>(*this); }

			T _t;
		};

		std::unique_ptr<concept> _p;
	};

	template <typename Type = Any, typename Payload = Any>
	class Action {
	public:
		template <class T>
		Action(const T& t) : _p(new concrete<T>(t)) {}

		Action(Action&&) = default;

		Action(const Action& action) : _p(action._p->copy()) {}

		Action& operator=(Action action) {
			_p = std::move(action._p);
			return *this;
		}

		Action& operator=(Action&&) = default;

		Type type() const { return _p->type(); }

		Payload payload() const { return _p->payload(); }

	private:
		struct concept {
			virtual ~concept() = default;

			virtual concept* copy() const = 0;

			virtual Type type() const = 0;

			virtual Payload payload() const = 0;
		};

		template <typename T>
		struct concrete : public concept {
			explicit concrete(T t) : _t(std::move(t)) {}

			concept* copy() const override { return new concrete(*this); }

			Type type() const override { return _t.type(); }

			Payload payload() const override { return _t.payload(); }

			T _t;
		};

		std::unique_ptr<concept> _p;
	};

	template <typename Disposed = std::function<bool()>, typename Disposable = std::function<void()>>
	class Unsubscribe {
	public:

		template <typename T>
		Unsubscribe(const T& t) : _p(new concrete<T>(t)) {}

		Unsubscribe(const Disposed& disposed, const Disposable& disposable) : _p(
			new concrete<DefaultUnsubscribe>(DefaultUnsubscribe{ disposed, disposable })
		) {}

		Unsubscribe(Unsubscribe&&) = default;

		Unsubscribe(const Unsubscribe& unsubscribe) : _p(unsubscribe._p->copy()) {}

		Unsubscribe& operator=(Unsubscribe unsubscribe) {
			_p = std::move(unsubscribe._p);
			return *this;
		}

		Unsubscribe& operator=(Unsubscribe&&) = default;

		Disposed disposed() const { return _p->disposed(); }

		Disposable disposable() const { return _p->disposable(); }

		void dispose() const { _p->disposable()(); }

	private:
		struct concept {
			virtual ~concept() = default;

			virtual concept* copy() const = 0;

			virtual Disposed disposed() const = 0;

			virtual Disposable disposable() const = 0;
		};

		template <typename T>
		struct concrete : public concept {
			explicit concrete(T t) : _t(std::move(t)) {}

			concept* copy() const override { return new concrete(*this); }

			Disposed disposed() const override { return _t.disposed(); }

			Disposable disposable() const override { return _t.disposable(); }

			T _t;
		};

		struct DefaultUnsubscribe {
			Disposed disposed() const { return _disposed; }
			Disposable disposable() const { return _disposer; }

			Disposed _disposed;
			Disposable _disposer;
		};

		std::unique_ptr<concept> _p;
	};

	template<typename State>
	using GetState = std::function<State()>;

	template<typename State>
	using Reducer = std::function<State(State, Action<>)>;

	template<typename State>
	using Listener = std::function<void(State)>;

	template<typename State>
	using Subscribe = std::function<Unsubscribe<>(Listener<State>)>;

	using Dispatch = std::function<Action<>(Action<>)>;

	using DispatchTransform = std::function<Dispatch(Dispatch)>;

	template <typename State>
	class Middleware {
	public:

		template <class T>
		Middleware(const T& t) : _p(new concrete<T>(t)) {}

		Middleware(const Dispatch& dispatch, const GetState<State>& getState) : _p(
			new concrete<DefaultMiddleware>(DefaultMiddleware{ dispatch, getState })
		) {}

		Middleware(Middleware&& middleware) = default;

		Middleware(const Middleware& middleware) : _p(middleware._p->copy()) {}

		Middleware& operator=(Middleware middleware) {
			_p = std::move(middleware._p);
			return *this;
		}

		Middleware& operator=(Middleware&&) = default;

		Dispatch dispatch() const { return _p->dispatch(); }

		GetState<State> getState() const { return _p->getState(); }

	private:
		struct concept {
			virtual ~concept() = default;

			virtual concept* copy() const = 0;

			virtual Dispatch dispatch() const = 0;

			virtual GetState<State> getState() const = 0;
		};

		template <typename T>
		struct concrete : public concept {
			explicit concrete(T t) : _t(std::move(t)) {}

			concept* copy() const override { return new concrete(*this); }

			Dispatch dispatch() const override { return _t.dispatch(); }

			GetState<State> getState() const override { return _t.getState(); }

			T _t;
		};

		struct DefaultMiddleware {
			Dispatch dispatch() const { return _dispatch; }
			GetState<State> getState() const { return _getState; }

			Dispatch _dispatch;
			GetState<State> _getState;
		};


		std::unique_ptr<concept> _p;
	};

	template<typename State>
	using MiddlewareDispatchTransform = std::function<DispatchTransform(Middleware<State>)>;

	template<typename State>
	using MiddlewareDispatchTransformChain = std::initializer_list<MiddlewareDispatchTransform<State>>;

	template <typename State>
	class Store {
	public:
		Store() = delete;

		Action<> dispatch(const Action<>& action) const {
			return _dispatch(action);
		}

		Unsubscribe<> subscribe(const Listener<State>& listener) const {
			return _subscribe(listener);
		}

		State state() const {
			return _state;
		}

		template <typename State>
		friend Store<State> createStore(
			const Reducer<State>& reducer,
			const State& preloadedState,
			const MiddlewareDispatchTransformChain<State>& middlewareDispatchTransformChain
		);

	private:

		Store(const Reducer<State>& reducer, const State& preloadedState) : _reducer(reducer), _state(preloadedState) {

			_subscribe = [&](Listener<State> listener) -> Unsubscribe<> {
				int id = _nextId++;
				_listeners[id] = listener;
				return Unsubscribe<> (
					[this, id]() { return _listeners.find(id) == std::end(_listeners); },
					[this, id]() { _listeners.erase(id); }
				);
			};

			_dispatch = [&](Action<> action) -> Action<> {

				if (_isDispatching) {
					return action;
				}

				_isDispatching = true;
				_state = _reducer(_state, action);
				_isDispatching = false;

				for(std::pair<int, Listener<State>> pair : _listeners) {
					pair.second(_state);
				}

				return action;
			};

		}

		Store(
			const Reducer<State>& reducer,
			const State& preloadedState,
			const MiddlewareDispatchTransformChain<State>& middlewareDispatchTransformChain
		) : Store(reducer, preloadedState) {
			
			_dispatch = std::accumulate(
				std::begin(middlewareDispatchTransformChain), 
				std::end(middlewareDispatchTransformChain), 
				_dispatch,
				[&](Dispatch dispatch, MiddlewareDispatchTransform<State> transform) -> Dispatch {
					const auto& middleware = Middleware<State>(_dispatch, [&]() {return _state; });
					return transform(middleware)(dispatch);
				}
			);
		}

		int _nextId{ 0 };
		bool _isDispatching{ false };
		std::unordered_map<int, Listener<State>> _listeners;
		State _state;
		Dispatch _dispatch;
		Reducer<State> _reducer;
		Subscribe<State> _subscribe;
	};

	template <typename State>
	Store<State> createStore(
		const Reducer<State>& reducer,
		const State& preloadedState,
		const MiddlewareDispatchTransformChain<State>& middlewareDispatchTransformChain = MiddlewareDispatchTransformChain<State>{}
	) {
		return Store<State>(reducer, preloadedState, middlewareDispatchTransformChain);
	}

}
