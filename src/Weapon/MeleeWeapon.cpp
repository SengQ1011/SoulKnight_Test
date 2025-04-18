//
// Created by tjx20 on 3/26/2025.
//
#include "Weapon/MeleeWeapon.hpp"

#include "Scene/SceneManager.hpp"
#include "Weapon/Slash.hpp"

MeleeWeapon::MeleeWeapon(const std::string &ImagePath, const std::shared_ptr<Animation> slashAnimation, const std::string &name, int damage, int energy, float criticalRate,
						int offset, float attackSpeed, float attackRange)
						: Weapon(ImagePath, name, damage, energy, criticalRate, offset, attackSpeed),
							m_slashAnimation(slashAnimation), m_attackRange(attackRange){}

void MeleeWeapon::attack(int damage) {
	ResetAttackTimer();  // 重置冷卻

	const auto characterType = m_currentOwner->GetType();
	auto tranform = m_currentOwner;
	const auto slash = std::make_shared<Slash>(characterType, m_slashAnimation, m_attackRange);
	slash->Init();
	// 加入渲染樹
	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	currentScene->GetRoot().lock()->AddChild(slash);  // 由 BulletManager 的 shared_ptr 加入
	currentScene->GetCamera().lock()->AddChild(slash);

	// 注冊到碰撞管理器
	currentScene->GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION)->RegisterNGameObject(slash);
}