//
// Created by tjx20 on 3/25/2025.
//

#ifndef INPUTCOMPONENT_HPP
#define INPUTCOMPONENT_HPP

#include "Components/Component.hpp"
#include "Observer.hpp"

class InputComponent : public Component, public InputObserver{
public:
	explicit InputComponent();
	~InputComponent() override = default;

	void onInputReceived(const std::set<char>& keys) override;
};

#endif //INPUTCOMPONENT_HPP
