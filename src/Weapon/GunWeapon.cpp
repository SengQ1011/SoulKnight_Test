//
// Created by tjx20 on 3/26/2025.
//

#include "Weapon/GunWeapon.hpp"

#include "Scene/SceneManager.hpp"

GunWeapon::GunWeapon(const std::string &ImagePath, const std::string& bulletImagePath,const std::string &name, int damage, int energy, float criticalRate,
						int offset, float attackInterval, float size, float speed)
						: Weapon(ImagePath, name, damage, energy, criticalRate, offset, attackInterval),
							m_bulletImagePath(bulletImagePath), m_bulletSize(size), m_bulletSpeed(speed){}


void GunWeapon::attack(const int damage) {
	ResetAttackTimer();  // 重置冷卻

	// 計算子彈方向（使用旋轉角度）
	const auto bulletDirection = glm::vec2(cos(this->m_Transform.rotation), sin(this->m_Transform.rotation));
	// 建立 Transform
	Util::Transform bulletTransform;
	bulletTransform.translation = this->m_WorldCoord;									// 子彈的位置
	bulletTransform.scale = glm::vec2(1.0f, 1.0f);									// 大小
	bulletTransform.rotation = glm::atan(bulletDirection.y, bulletDirection.x);        // 子彈的角度

	const auto characterType = m_currentOwner->GetType();
	auto numRebound = m_currentOwner->GetComponent<AttackComponent>(ComponentType::ATTACK)->GetNumRebound();

	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();

	// if (const auto currentRoom = currentScene->GetCurrentRoom();
	// 	currentRoom != nullptr)
	// {
	// 	LOG_DEBUG("HAS ROOM");
	// 	attackManager = currentRoom->GetAttackManager();
	// 	}
	// else
	// {
	// 	LOG_DEBUG("NO ROOM");
	// 	attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	// }
	const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	attackManager->spawnProjectile(characterType, bulletTransform, bulletDirection, m_bulletSize, damage, m_bulletImagePath, m_bulletSpeed, numRebound);
 }
