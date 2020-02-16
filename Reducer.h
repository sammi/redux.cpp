#pragma once
#include "Action.h"

namespace App {
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
}