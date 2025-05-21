//
// Created by tjx20 on 3/26/2025.
//

#include "Weapon/GunWeapon.hpp"

#include "Attack/AttackManager.hpp"
#include "Components/AttackComponent.hpp"
#include "Creature/Character.hpp"
#include "RandomUtil.hpp"
#include "Scene/SceneManager.hpp"

GunWeapon::GunWeapon(const GunWeaponInfo& gunWeaponInfo)
		: Weapon(gunWeaponInfo),
			m_bulletImagePath(gunWeaponInfo.bulletImagePath), m_numOfBullets(gunWeaponInfo.numOfBullets),
			m_bulletOffset(gunWeaponInfo.bulletOffset), m_bulletSize(gunWeaponInfo.bulletSize),
			m_bulletSpeed(gunWeaponInfo.bulletSpeed), m_bulletCanReboundBySword(gunWeaponInfo.bulletCanReboundBySword),
			m_bulletIsBubble(gunWeaponInfo.bulletIsBubble), m_bulletHaveBubbleTrail(gunWeaponInfo.haveBubbleTrail),
			m_bubbleImagePath(gunWeaponInfo.bubbleImagePath),
			m_bulletHaveEffectAttack(gunWeaponInfo.haveEffectAttack), m_effectAttackSize(gunWeaponInfo.effectAttackSize),
			m_effectAttackDamage(gunWeaponInfo.effectAttackDamage), m_bullet_EffectAttack(gunWeaponInfo.effect)
{
	m_AttackType = AttackType::GUN;
}

void GunWeapon::attack(const int damage) {
	ResetAttackTimer();  // 重置冷卻

	// 計算子彈方向（旋轉角度是朝右的單位向量）
	glm::vec2 bulletDirection = glm::vec2(cos(m_Transform.rotation), sin(m_Transform.rotation));
	if (m_Transform.scale.x < 0) {
		bulletDirection.x *= -1;
	}

	// 用 bulletDirection * offset 做子彈偏移位置
	glm::vec2 offset = bulletDirection * m_bulletOffset;

	// 建立 Transform
	Util::Transform bulletTransform;
	bulletTransform.translation = m_WorldCoord + offset; // 子彈生成位置
	bulletTransform.rotation = glm::atan(bulletDirection.y, bulletDirection.x); // 子彈旋轉

	const auto characterType = m_currentOwner->GetType();
	auto numRebound = m_currentOwner->GetComponent<AttackComponent>(ComponentType::ATTACK)->GetNumRebound();

	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();

	const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	float spreadAngle = glm::radians(35.0f);  // 總共散佈的角度
	int numBullets = m_numOfBullets;

	for (int i = 0; i < numBullets; ++i) {
		float angleOffset = 0.0f;
		if (numBullets > 1) {
			angleOffset = spreadAngle * ((static_cast<float>(i) / (numBullets - 1)) - 0.5f);
		}
		float baseRotation = glm::atan(bulletDirection.y, bulletDirection.x);
		float randomJitter = 0;

		if (i != 0 && m_weaponType == WeaponType::SHOTGUN)
		{
			const glm::vec2 offset = RandomUtil::RandomDirectionInsideUnitCircle() * RandomUtil::RandomFloatInRange(0.0f, 3.0f);
			bulletTransform.translation += offset;
			randomJitter = RandomUtil::RandomFloatInRange(-glm::radians(7.0f), glm::radians(7.0f)); // ±7度抖動
		}
		float newRotation = baseRotation + angleOffset + randomJitter;

		// 以旋轉角度轉回方向向量
		glm::vec2 newDirection = glm::vec2(cos(newRotation), sin(newRotation));

		// 更新 Transform
		bulletTransform.rotation = newRotation;

		ProjectileInfo bulletInfo;
		bulletInfo.type = m_currentOwner->GetType();
		bulletInfo.attackTransform = bulletTransform;
		bulletInfo.direction = newDirection;
		bulletInfo.size = m_bulletSize;
		bulletInfo.damage = damage;

		bulletInfo.imagePath = m_bulletImagePath;
		bulletInfo.speed = m_bulletSpeed;
		bulletInfo.numRebound = numRebound;
		bulletInfo.canReboundBySword = m_bulletCanReboundBySword;
		bulletInfo.isBubble = m_bulletIsBubble;
		bulletInfo.bubbleImagePath = m_bubbleImagePath;
		bulletInfo.bubbleTrail = m_bulletHaveBubbleTrail;
		bulletInfo.haveEffectAttack = m_bulletHaveEffectAttack;
		bulletInfo.effectAttackSize = m_effectAttackSize;
		bulletInfo.effectAttackDamage = m_effectAttackDamage;
		bulletInfo.effect = m_bullet_EffectAttack;

		attackManager->spawnProjectile(bulletInfo);
	}
 }
