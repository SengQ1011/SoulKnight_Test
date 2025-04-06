//
// Created by tjx20 on 4/5/2025.
//

#include "Components/AttackAI.hpp"

AttackAI::AttackAI(int monsterPoint): AIComponent(AIType::ATTACK, monsterPoint) {}

// TODO
// void AttackAI::Update() {
// 	if (!m_target.expired()) {
// 		m_attackTimer -= GetDeltaTime();
// 		if (m_attackTimer <= 0) {
// 			ExecuteAttack();
// 			m_attackTimer = m_attackCooldown;
// 		}
// 	}
// }

// TODO
// void AttackAI::OnDetectTarget(Character* target) {
// 	m_target = target->shared_from_this();
// 	ShowExclamationMark(); // 顯示感嘆號提示
// }