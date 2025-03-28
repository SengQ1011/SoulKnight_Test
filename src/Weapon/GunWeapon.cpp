//
// Created by tjx20 on 3/26/2025.
//

#include "Weapon/GunWeapon.hpp"

GunWeapon::GunWeapon(const std::string &ImagePath, const std::string& bulletImagePath,const std::string &name, int damage, int energy, float criticalRate,
						int offset, float attackSpeed, float size)
						: Weapon(ImagePath, bulletImagePath,name, damage, energy, criticalRate, offset, attackSpeed),
							m_bulletSize(size){}


void GunWeapon::attack(int damage) {
	if(!CanAttack()) return;

	lastAttackTime = m_attackColdDown;  // 重置冷卻

	float bulletSpeed = 150.0f;
	// 建立 Transform
	Util::Transform bulletTransform;
	bulletTransform.translation = this->m_WorldCoord;  // 設定為槍的位置
	bulletTransform.rotation = this->m_Transform.rotation;        // 設定為槍的角度

	// 計算子彈方向（使用旋轉角度）
	glm::vec2 bulletDirection = glm::vec2(cos(this->m_Transform.rotation), sin(this->m_Transform.rotation));

	BulletManager::GetInstance().spawnBullet(m_bulletImagePath,bulletTransform, bulletDirection, bulletSpeed, damage);
 }
