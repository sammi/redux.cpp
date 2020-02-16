#pragma once

#include <string>

namespace App {
	class State {
	public:
		explicit State(int counter = 0) : _counter(counter) {};
		int counter() const { return _counter; };
		std::string toString() const { return "counter: " + std::to_string(_counter); }
	private:
		int _counter;
	};
}
