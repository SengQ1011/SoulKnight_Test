//
// Created by tjx20 on 4/5/2025.
//

#ifndef ATTACKAI_HPP
#define ATTACKAI_HPP

#include "Components/AiComponent.hpp"

// AttackAI.hpp
class AttackAI : public AIComponent {
public:
	explicit AttackAI(int monsterPoint);

	void Update() override {};

	// void OnDetectTarget(Character* target) override;

private:
	// void ExecuteAttack() {
	// 	if (auto owner = GetOwner<Character>()) {
			// TODO: owner->GetComponent<AttackComponent>()->TryAttack();
	// 	}
	// }

	float m_attackCooldown = 2.0f;
	float m_attackTimer = 0;
	float m_warningDuration = 1.0f;
};

#endif //ATTACKAI_HPP
