//
// Created by tjx20 on 3/26/2025.
//

#include "Weapon/GunWeapon.hpp"

#include "Attack/AttackManager.hpp"
#include "Components/AttackComponent.hpp"
#include "Creature/Character.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "RandomUtil.hpp"
#include "Scene/SceneManager.hpp"

GunWeapon::GunWeapon(const GunWeaponInfo &gunWeaponInfo) :
	Weapon(gunWeaponInfo), m_numOfBullets(gunWeaponInfo.numOfBullets),
	m_bulletCanTracking(gunWeaponInfo.bulletCanTracking), m_projectileInfo(gunWeaponInfo.defaultProjectileInfo)
{
	m_AttackType = AttackType::PROJECTILE;
}

void GunWeapon::attack(const int damage, bool isCriticalHit)
{
	ResetAttackTimer(); // 重置冷卻

	// 播放對應的槍械音效
	switch (m_weaponType)
	{
	case WeaponType::PISTOL:
		AudioManager::GetInstance().PlaySFX("gun_pistol");
		break;
	case WeaponType::SHOTGUN:
		AudioManager::GetInstance().PlaySFX("gun_shotgun");
		break;
	case WeaponType::ROCKET_LAUNCHER:
		AudioManager::GetInstance().PlaySFX("gun_rocket");
		break;
	default:
		// 其他槍械類型暫時不播放特定音效
		break;
	}

	// 計算子彈方向（旋轉角度是朝右的單位向量）
	glm::vec2 bulletDirection = glm::vec2(cos(m_Transform.rotation), sin(m_Transform.rotation));
	if (m_Transform.scale.x < 0)
	{
		bulletDirection.x *= -1;
	}

	// 用 bulletDirection * offset 做子彈偏移位置
	glm::vec2 offset = bulletDirection * m_attackInitPositionOffset;

	// 建立 Transform
	Util::Transform bulletTransform;
	bulletTransform.translation = m_WorldCoord + offset; // 子彈生成位置
	bulletTransform.rotation = glm::atan(bulletDirection.y, bulletDirection.x); // 子彈旋轉

	const auto characterType = m_currentOwner->GetType();
	int numRebound = 0;
	if (m_projectileInfo.canReboundBySword)
		numRebound = m_currentOwner->GetComponent<AttackComponent>(ComponentType::ATTACK)->GetNumRebound();

	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();

	const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	float spreadAngle = glm::radians(35.0f); // 總共散佈的角度
	int numBullets = m_numOfBullets;

	std::shared_ptr<nGameObject> target = nullptr;
	if (const auto followComp = this->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
		target = followComp->GetTarget();

	for (int i = 0; i < numBullets; ++i)
	{
		float angleOffset = 0.0f;
		if (numBullets > 1)
		{
			angleOffset = spreadAngle * ((static_cast<float>(i) / (numBullets - 1)) - 0.5f);
		}
		float baseRotation = glm::atan(bulletDirection.y, bulletDirection.x);
		float randomJitter = 0;

		if (i != 0 && m_weaponType == WeaponType::SHOTGUN)
		{
			const glm::vec2 offset =
				RandomUtil::RandomDirectionInsideUnitCircle() * RandomUtil::RandomFloatInRange(0.0f, 3.0f);
			bulletTransform.translation += offset;
			randomJitter = RandomUtil::RandomFloatInRange(-glm::radians(7.0f), glm::radians(7.0f)); // ±7度抖動
		}
		float newRotation = baseRotation + angleOffset + randomJitter;

		// 以旋轉角度轉回方向向量
		glm::vec2 newDirection = glm::vec2(cos(newRotation), sin(newRotation));

		// 更新 Transform
		bulletTransform.rotation = newRotation;

		ProjectileInfo projectileInfo = m_projectileInfo;
		projectileInfo.type = m_currentOwner->GetType();
		projectileInfo.attackTransform = bulletTransform;
		projectileInfo.direction = newDirection;
		projectileInfo.damage = damage;
		projectileInfo.isCriticalHit = isCriticalHit;
		if (m_bulletCanTracking)
		{
			projectileInfo.target = target;
		}

		attackManager->spawnProjectile(projectileInfo);
	}
}
