//
// Created by tjx20 on 3/26/2025.
//
#include "Weapon/MeleeWeapon.hpp"

#include "Attack/AttackManager.hpp"
#include "Components/AttackComponent.hpp"
#include "Creature/Character.hpp"
#include "Scene/SceneManager.hpp"

MeleeWeapon::MeleeWeapon(const std::string &ImagePath, const std::string &name, int damage, int energy, float criticalRate,
						int offset, float attackInterval, float attackRange)
						: Weapon(ImagePath, name, damage, energy, criticalRate, offset, attackInterval),
							m_attackRange(attackRange){}

void MeleeWeapon::attack(int damage) {
	ResetAttackTimer();  // 重置冷卻

	// 計算攻擊方向的中心點（使用旋轉角度）
	const auto bulletDirection = glm::vec2(cos(this->m_Transform.rotation), sin(this->m_Transform.rotation));
	// 建立 Transform
	Util::Transform slashTransform;

	glm::vec2 offset = (m_currentOwner->m_Transform.scale.x < 0) ? glm::vec2(16.0f, 0.0f) : glm::vec2(-16.0f, 0.0f);
	slashTransform.translation = this->m_WorldCoord + offset;							// 揮擊的位置
	slashTransform.scale = glm::vec2(0.6f, 0.6f);									// 大小
	slashTransform.rotation = glm::atan(bulletDirection.y, bulletDirection.x);			// 刀揮擊的中心角度

	const auto characterType = m_currentOwner->GetType();

	auto canReflect = m_currentOwner->GetComponent<AttackComponent>(ComponentType::ATTACK)->GetReflectBullet();

	if(const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock())
	{
		const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
		attackManager->spawnEffectAttack(characterType, slashTransform, bulletDirection, m_attackRange, damage, canReflect,EffectAttackType::SLASH);
	} else
	{
		LOG_ERROR("Can't find currentScene");
	}

	// TODO:揮刀動作

}