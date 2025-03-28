//
// Created by tjx20 on 3/25/2025.
//

#include "Components/InputComponent.hpp"

#include "Components/AttackComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Util/Time.hpp"

InputComponent::InputComponent() { LOG_DEBUG("InputComponent Success"); }


void InputComponent::onInputReceived(const std::set<char>& keys)
{
	auto character = GetOwner<Character>();
	if (!character) return;

	auto stateComponent = character->GetComponent<StateComponent>(ComponentType::STATE);
	auto attackComponent = character->GetComponent<AttackComponent>(ComponentType::ATTACK);
	auto animationComponent = character->GetComponent<AnimationComponent>(ComponentType::ANIMATION);
	auto m_currentAnimation = animationComponent->GetCurrentAnimation();

	// movement移動
	float deltaTime = Util::Time::GetDeltaTimeMs();
	glm::vec2 movement(0.0f, 0.0f);
	if (keys.count('W')) movement.y += 1.0f;
	if (keys.count('S')) movement.y -= 1.0f;
	if (keys.count('A')) movement.x -= 1.0f;
	if (keys.count('D')) movement.x += 1.0f;

	if (movement.x != 0.0f || movement.y != 0.0f) {
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
	} else {
		stateComponent->SetState(State::STANDING);
	}

	// 武器/攻擊attackComponent
	if (attackComponent)
	{
		auto currentWeapon = attackComponent->GetCurrentWeapon();
		if (keys.count('J')) {
			attackComponent->TryAttack();
		}

		//if (keys.count('U')) attackComponent->switchWeapon();
	}

}