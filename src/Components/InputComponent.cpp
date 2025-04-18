//
// Created by tjx20 on 3/25/2025.
//

#include "Components/InputComponent.hpp"

#include "Components/AttackComponent.hpp"
#include "Components/FollowerComponent.hpp"
#include "Components/SkillComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Scene/SceneManager.hpp"

InputComponent::InputComponent() {}

void InputComponent::onInputReceived(const std::set<char>& keys)
{
	auto character = GetOwner<Character>();
	if (!character || !character->IsActive()) return;

	auto stateComponent = character->GetComponent<StateComponent>(ComponentType::STATE);
	auto skillComponent = character->GetComponent<SkillComponent>(ComponentType::SKILL);
	auto movementComponent = character->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
	auto attackComponent = character->GetComponent<AttackComponent>(ComponentType::ATTACK);
	auto animationComponent = character->GetComponent<AnimationComponent>(ComponentType::ANIMATION);
	auto m_currentAnimation = animationComponent->GetCurrentAnimation();

	// movement移動
	glm::vec2 movement(0.0f, 0.0f);
	if (keys.count('W')) movement.y += 1.0f;
	if (keys.count('S')) movement.y -= 1.0f;
	if (keys.count('A')) movement.x -= 1.0f;
	if (keys.count('D')) movement.x += 1.0f;

	if (movement.x != 0.0f || movement.y != 0.0f) {
		const float ratio = 0.2f;
		// movementComp中會考慮deltaTime了
		const glm::vec2 deltaDisplacement = normalize(movement) * ratio; //normalize為防止斜向走速度是根號2

		movementComponent->SetDesiredDirection(deltaDisplacement);
		stateComponent->SetState(State::MOVING);
	} else {
		// 無輸入時，清空方向向量
		movementComponent->SetDesiredDirection(glm::vec2(0.0f));
		stateComponent->SetState(State::STANDING);
	}

	// 武器/攻擊attackComponent
	if (attackComponent)
	{
		auto currentWeapon = attackComponent->GetCurrentWeapon();
		if (keys.count('J')) {
			attackComponent->TryAttack();
		}
		//if (keys.count('E')) attackComponent->switchWeapon();
	}
	// 使用技能
	if (keys.count('U')) {
		LOG_DEBUG("Skill Try Executed");
		if(skillComponent->ExecuteSkill())
		{
			stateComponent->SetState(State::SKILL);
		}
	}
}