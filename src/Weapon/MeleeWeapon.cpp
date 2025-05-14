//
// Created by tjx20 on 3/26/2025.
//
#include "Weapon/MeleeWeapon.hpp"

#include "Attack/AttackManager.hpp"
#include "Components/AttackComponent.hpp"
#include "Creature/Character.hpp"
#include "Scene/SceneManager.hpp"

MeleeWeapon::MeleeWeapon(const std::string &ImagePath, const std::string &name, int damage, int energy, float criticalRate,
						int offset, float attackInterval, float attackRange, bool isSword, const EffectAttackType type)
						: Weapon(ImagePath, name, damage, energy, criticalRate, offset, attackInterval),
							m_attackRange(attackRange),  m_effectAttackType(type)
{
	SetIsSword(isSword);
}
void MeleeWeapon::attack(int damage) {
	ResetAttackTimer();  // 重置冷卻

	// 揮刀動作
	if (const auto followerComp = this->GetComponent<FollowerComponent>(ComponentType::FOLLOWER)) {
		followerComp->StartAttackAction();
	}

	// 原始旋轉角度
	float rawRotation = this->m_Transform.rotation;

	// 還原初始角度偏移（只對劍適用）
	if (m_IsSword) {
		const bool facingLeft = m_currentOwner->m_Transform.scale.x < 0.0f;
		if (const auto follower = this->GetComponent<FollowerComponent>(ComponentType::FOLLOWER)) {
			const float prepOffset = glm::radians(follower->GetBaseRotationDegrees());
			rawRotation -= facingLeft ? -prepOffset : prepOffset;
		}
	}
	// 計算攻擊方向的中心點（使用旋轉角度）
	const auto slashDirection = glm::vec2(cos(rawRotation), sin(rawRotation));
	// normalize rotation to [-π, π]
	const float angle = glm::atan(slashDirection.y, slashDirection.x);

	// 建立 Transform
	Util::Transform slashTransform;
	float distance = 0.0f;
	if(m_effectAttackType == EffectAttackType::SLASH) distance = 16.0f;
	else if (m_effectAttackType == EffectAttackType::LUNGE) distance = 16.0f;
	else if (m_effectAttackType == EffectAttackType::SHOCKWAVE) distance = 25.0f;
	// facingLeft
	if(std::cos(angle) < 0.0f)distance += 5.0f;

	glm::vec2 offset;

	// 定義方向閾值
	if (angle > -glm::pi<float>() / 8 && angle <= glm::pi<float>() / 8) {
		offset = glm::vec2(distance, 0.0f);  // 右→
	} else if (angle > glm::pi<float>() / 8 && angle <= 3 * glm::pi<float>() / 8) {
		offset = glm::vec2(distance, distance);  // 右上↗
	} else if (angle > 3 * glm::pi<float>() / 8 && angle <= 5 * glm::pi<float>() / 8) {
		offset = glm::vec2(0.0f, distance);  // 上↑
	} else if (angle > 5 * glm::pi<float>() / 8 && angle <= 7 * glm::pi<float>() / 8) {
		offset = glm::vec2(-distance, distance);  // 左上↖
	} else if (angle > 7 * glm::pi<float>() / 8 || angle <= -7 * glm::pi<float>() / 8) {
		offset = glm::vec2(-distance, 0.0f);  // 左←
	} else if (angle > -7 * glm::pi<float>() / 8 && angle <= -5 * glm::pi<float>() / 8) {
		offset = glm::vec2(-distance, -distance);  // 左下↙
	} else if (angle > -5 * glm::pi<float>() / 8 && angle <= -3 * glm::pi<float>() / 8) {
		offset = glm::vec2(0.0f, -distance);  // 下↓
	} else {
		offset = glm::vec2(distance, -distance);  // 右下↘
	}
	slashTransform.translation = this->m_WorldCoord + offset;							// 揮擊的位置

	if (m_effectAttackType == EffectAttackType::SHOCKWAVE){
		slashTransform.rotation = 0.0f;
		slashTransform.scale.y = std::abs(slashTransform.scale.y);
	}
	else {
		slashTransform.rotation = glm::atan(slashDirection.y, slashDirection.x);		// 武器揮擊的中心角度
		// y軸翻轉
		slashTransform.scale.y = (m_currentOwner->m_Transform.scale.x > 0) ?
			std::abs(slashTransform.scale.y): -std::abs(slashTransform.scale.y);
	}

	const auto characterType = m_currentOwner->GetType();
	auto canReflect = m_currentOwner->GetComponent<AttackComponent>(ComponentType::ATTACK)->GetReflectBullet();

	if(const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock()) {
		const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
		attackManager->spawnEffectAttack(characterType, slashTransform, slashDirection, m_attackRange, damage, canReflect, m_IsSword, m_effectAttackType);
	} else {
		LOG_ERROR("Can't find currentScene");
	}
}