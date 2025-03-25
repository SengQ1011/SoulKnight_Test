//
// Created by tjx20 on 3/24/2025.
//

#include "Components/StateComponent.hpp"

StateComponent::StateComponent() {}

void StateComponent::Init() {
	m_currentState = State::STANDING;
	LOG_DEBUG("Creating State Component");
}

void StateComponent::SetState(State newState) {
	if (newState != m_currentState) {
		LOG_DEBUG("Changing State to {}",std::to_string(static_cast<int>(newState)));
		m_currentState = newState;
		auto character = GetOwner<nGameObject>();
		if (character) {
			auto animationComponent = character->GetComponent<AnimationComponent>(ComponentType::ANIMATION);
			auto m_currentAnimation = animationComponent->GetCurrentAnimation();
			// 根据当前状态切换动画
			if (m_currentAnimation) {
				LOG_DEBUG("changing animation");
				m_currentAnimation->PlayAnimation(false);
				animationComponent->SetAnimation(m_currentState);
			}
		}
	}
}