//
// Created by tjx20 on 3/26/2025.
//

#include "Weapon/GunWeapon.hpp"

#include "Scene/SceneManager.hpp"

GunWeapon::GunWeapon(const std::string &ImagePath, const std::string& bulletImagePath,const std::string &name, int damage, int energy, float criticalRate,
						int offset, float attackSpeed, float size)
						: Weapon(ImagePath, bulletImagePath,name, damage, energy, criticalRate, offset, attackSpeed),
							m_bulletSize(size){}


void GunWeapon::attack(int damage) {
	lastAttackTime = m_attackColdDown;  // 重置冷卻

	float bulletSpeed = 170.0f;
	// 計算子彈方向（使用旋轉角度）
	glm::vec2 bulletDirection = glm::vec2(cos(this->m_Transform.rotation), sin(this->m_Transform.rotation));
	// 建立 Transform
	Util::Transform bulletTransform;
	bulletTransform.translation = this->m_WorldCoord;									// 子彈的位置
	bulletTransform.scale = glm::vec2(0.6f, 0.6f);									// 大小
	bulletTransform.rotation = glm::atan(bulletDirection.y, bulletDirection.x);        // 子彈的角度
	auto characterType = m_currentOwner->GetType();

	auto bulletManager = SceneManager::GetInstance().GetCurrentScene().lock()->GetManager<BulletManager>(ManagerTypes::BULLET);
	bulletManager->spawnBullet(characterType, m_bulletImagePath,bulletTransform, bulletDirection, m_bulletSize, bulletSpeed, damage);
 }
