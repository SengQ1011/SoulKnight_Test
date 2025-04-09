//
// Created by tjx20 on 4/5/2025.
//

#ifndef ATTACKAI_HPP
#define ATTACKAI_HPP

#include "Components/AiComponent.hpp"

// AttackAI.hpp
class AttackAI final : public AIComponent {
public:
	explicit AttackAI(int monsterPoint);
	~AttackAI()override = default;

	void Update() override;

private:
	void ExecuteAttack() const{
		if (const auto owner = GetOwner<Character>()) {
			owner->GetComponent<AttackComponent>(ComponentType::ATTACK)->TryAttack();
		}
	}

	float m_attackCooldown = 2.0f;
	float m_attackTimer = 0;
	float m_warningDuration = 1.0f;
};

#endif //ATTACKAI_HPP
