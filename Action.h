#pragma once

namespace App {
	enum class ActionType {
		increment,
		decrement,
		thunk
	};

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
}
