//
// Created by tjx20 on 3/26/2025.
//

#include "Weapon/GunWeapon.hpp"
#include "Components/AttackComponent.hpp"
#include "Attack/AttackManager.hpp"
#include "Creature/Character.hpp"
#include "Scene/SceneManager.hpp"

GunWeapon::GunWeapon(const GunWeaponInfo& gunWeaponInfo)
		: Weapon(gunWeaponInfo),
			m_bulletImagePath(gunWeaponInfo.bulletImagePath), m_bulletSize(gunWeaponInfo.bulletSize),
			m_bulletSpeed(gunWeaponInfo.bulletSpeed), m_bulletCanReboundBySword(gunWeaponInfo.bulletCanReboundBySword),
			m_bulletIsBubble(gunWeaponInfo.bulletIsBubble), m_bulletHaveBubbleTrail(gunWeaponInfo.haveBubbleTrail),
			m_bubbleImagePath(gunWeaponInfo.bubbleImagePath)
{
	m_AttackType = AttackType::GUN;
}

void GunWeapon::attack(const int damage) {
	ResetAttackTimer();  // 重置冷卻

	// 計算子彈方向（使用旋轉角度）
	const auto bulletDirection = glm::vec2(cos(this->m_Transform.rotation), sin(this->m_Transform.rotation));
	// 建立 Transform
	Util::Transform bulletTransform;
	bulletTransform.translation = this->m_WorldCoord;									// 子彈的位置
	bulletTransform.rotation = glm::atan(bulletDirection.y, bulletDirection.x);        // 子彈的角度

	const auto characterType = m_currentOwner->GetType();
	auto numRebound = m_currentOwner->GetComponent<AttackComponent>(ComponentType::ATTACK)->GetNumRebound();

	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();

	const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	attackManager->spawnProjectile(characterType, bulletTransform, bulletDirection, m_bulletSize, damage, m_bulletImagePath, m_bulletSpeed, numRebound, m_bulletCanReboundBySword, m_bulletIsBubble, m_bulletHaveBubbleTrail, m_bubbleImagePath);
 }
