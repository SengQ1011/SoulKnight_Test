//
// Created by tjx20 on 3/24/2025.
//

#include "Components/StateComponent.hpp"
#include "Creature/Character.hpp"

StateComponent::StateComponent() {}

void StateComponent::Init() {
	m_currentState = State::STANDING;
}

void StateComponent::SetState(State newState) {
	if (newState != m_currentState) {
		auto character = GetOwner<Character>();
		if (character) {
			auto animationComponent = character->GetComponent<AnimationComponent>(ComponentType::ANIMATION);
			auto m_currentAnimation = animationComponent->GetCurrentAnimation();
			if (character->GetCharacterName() == "Knight" && newState == State::SKILL)
			{
				animationComponent->SetSkillEffect(true);
			}
			else {
				m_currentState = newState;
				LOG_DEBUG("Changing State to {}",std::to_string(static_cast<int>(newState)));

				// 根据当前状态切换动画
				if (m_currentAnimation) {
					m_currentAnimation->PlayAnimation(false);
					animationComponent->SetAnimation(m_currentState);
				}

			}
		}
	}
}