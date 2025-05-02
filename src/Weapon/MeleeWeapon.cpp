//
// Created by tjx20 on 3/26/2025.
//
#include "Weapon/MeleeWeapon.hpp"

#include "Attack/AttackManager.hpp"
#include "Components/AttackComponent.hpp"
#include "Creature/Character.hpp"
#include "Scene/SceneManager.hpp"

MeleeWeapon::MeleeWeapon(const std::string &ImagePath, const std::string &name, int damage, int energy, float criticalRate,
						int offset, float attackInterval, float attackRange, const EffectAttackType type)
						: Weapon(ImagePath, name, damage, energy, criticalRate, offset, attackInterval),
							m_attackRange(attackRange), m_effectAttackType(type){}

void MeleeWeapon::attack(int damage) {
	ResetAttackTimer();  // 重置冷卻

	// 揮刀動作
	if (const auto followerComp = this->GetComponent<FollowerComponent>(ComponentType::FOLLOWER)) {
		followerComp->StartAttackAction();
	}

	// 計算攻擊方向的中心點（使用旋轉角度）
	const auto slashDirection = glm::vec2(cos(this->m_Transform.rotation), sin(this->m_Transform.rotation));
	// 建立 Transform
	Util::Transform slashTransform;
	slashTransform.scale = glm::vec2(1.0f);
	glm::vec2 offset;
	// normalize rotation to [-π, π]
	const float angle = glm::atan(slashDirection.y, slashDirection.x);  // already normalized
	// 定義方向閾值
	if (angle > -glm::pi<float>() / 4 && angle <= glm::pi<float>() / 4) {
		// → 右
		offset = glm::vec2(16.0f, 0.0f);
	} else if (angle > glm::pi<float>() / 4 && angle <= 3 * glm::pi<float>() / 4) {
		// ↑ 上
		offset = glm::vec2(0.0f, 16.0f);
	} else if (angle <= -glm::pi<float>() / 4 && angle > -3 * glm::pi<float>() / 4) {
		// ↓ 下
		offset = glm::vec2(0.0f, -16.0f);
	} else {
		// ← 左
		offset = glm::vec2(-18.0f, 0.0f);
	}
	slashTransform.translation = this->m_WorldCoord + offset;							// 揮擊的位置
	slashTransform.rotation = glm::atan(slashDirection.y, slashDirection.x);			// 刀揮擊的中心角度
	// y軸翻轉
	slashTransform.scale.y = (m_currentOwner->m_Transform.scale.x > 0) ?
		std::abs(slashTransform.scale.y): -std::abs(slashTransform.scale.y);

	const auto characterType = m_currentOwner->GetType();

	auto canReflect = m_currentOwner->GetComponent<AttackComponent>(ComponentType::ATTACK)->GetReflectBullet();

	if(const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock()) {
		const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
		attackManager->spawnEffectAttack(characterType, slashTransform, slashDirection, m_attackRange, damage, canReflect, m_effectAttackType);
	} else {
		LOG_ERROR("Can't find currentScene");
	}
}