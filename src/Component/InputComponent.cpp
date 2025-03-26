//
// Created by tjx20 on 3/25/2025.
//

#include "Components/InputComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Util/Time.hpp"

InputComponent::InputComponent() { LOG_DEBUG("InputComponent Success"); }


void InputComponent::onInputReceived(const std::set<char>& keys) {
	float deltaTime = Util::Time::GetDeltaTimeMs();
	glm::vec2 movement(0.0f, 0.0f);
	if (keys.count('W')) movement.y += 1.0f;
	if (keys.count('S')) movement.y -= 1.0f;
	if (keys.count('A')) movement.x -= 1.0f;
	if (keys.count('D')) movement.x += 1.0f;
	auto character = GetOwner<Character>();
	auto stateComponent = character->GetComponent<StateComponent>(ComponentType::STATE);
	auto animationComponent = character->GetComponent<AnimationComponent>(ComponentType::ANIMATION);
	auto m_currentAnimation = animationComponent->GetCurrentAnimation();
	if (movement.x != 0.0f || movement.y != 0.0f) {
		if(character) {
			const float ratio = 0.2f;
			const glm::vec2 deltaDisplacement = normalize(movement) * ratio * deltaTime; //normalize為防止斜向走速度是根號2
			auto movementComponent = character->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
			if ((movement.x < 0 && character->m_Transform.scale.x > 0) ||
				(movement.x > 0 && character->m_Transform.scale.x < 0))
			{
				character->m_Transform.scale.x *= -1.0f;
			}
			movementComponent->SetAcceleration(deltaDisplacement);
			//LOG_DEBUG("Location:{}", movementComponent->GetPosition());
			stateComponent->SetState(State::MOVING);
		}
	}else {
		stateComponent->SetState(State::STANDING);
	}
}