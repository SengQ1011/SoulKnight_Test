//
// Created by tjx20 on 3/25/2025.
//

#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <set>

// === 觀察者Interface ===
class Observer {
public:
	virtual ~Observer() = default; // 讓觀察者類別可以被正確釋放

	virtual void onInputReceived(const std::set<char>& keys) = 0;
};

#endif //OBSERVER_HPP
