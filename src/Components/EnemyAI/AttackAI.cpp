//
// Created by tjx20 on 4/5/2025.
//

#include "Components/EnemyAI/AttackAI.hpp"

AttackAI::AttackAI(int monsterPoint): AIComponent(AIType::ATTACK, monsterPoint) {}

void AttackAI::Update() {
	auto enemy = GetOwner<Character>();
	if(!enemy)return;
	if(m_Target.expired()) return;

	if (auto target = m_Target.lock()) {
		m_attackTimer -= Util::Time::GetDeltaTimeMs() / 1000.0f;
		if (m_attackTimer <= 0) {
			ExecuteAttack();
			m_attackTimer = m_attackCooldown;
		}
	}
}
