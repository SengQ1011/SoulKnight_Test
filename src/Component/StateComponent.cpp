//
// Created by tjx20 on 3/24/2025.
//

#include "Components/StateComponent.hpp"

StateComponent::StateComponent() {
	LOG_DEBUG("Creating State Component");
	SetState(State::STANDING);
}

void StateComponent::SetState(State newState) {
	//m_prevState = m_currentState; 切換動畫了才更新(animationComponent)
	m_currentState = newState;
}

void StateComponent::SetPrevState(State State) {
	m_prevState = m_currentState;
}