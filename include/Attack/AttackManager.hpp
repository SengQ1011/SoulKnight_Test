//
// Created by tjx20 on 3/26/2025.
//

#ifndef ATTACKMANAGER_HPP
#define ATTACKMANAGER_HPP

#include "Attack/ProjectilePool.hpp"
#include "Attack/EffectAttackPool.hpp"
#include "Util/Time.hpp"

class AttackManager {
public:
	// 构造函数与析构函数
	AttackManager() = default;
	~AttackManager() = default;

	// 禁止拷贝与赋值
	AttackManager(const AttackManager&) = delete;
	AttackManager& operator=(const AttackManager&) = delete;

	// 更新所有攻擊物件
	void Update();

	//----Getter----
	const std::vector<std::shared_ptr<Projectile>>& GetProjectiles() const { return m_projectiles; }
	const std::vector<std::shared_ptr<EffectAttack>>& GetEffects() const { return m_effects; }

	// 射出子彈類 OR 斬擊動畫
	void spawnProjectile(const CharacterType type, const Util::Transform& transform, glm::vec2 direction, float size, int damage, const std::string& bulletImagePath, float speed, int numRebound);
	void spawnEffectAttack(const CharacterType type, const Util::Transform& transform, glm::vec2 direction, float size, int damage, bool canReflect, EffectAttackType effectType);


private:
	std::vector<std::shared_ptr<Projectile>> m_projectiles;
	std::deque<std::shared_ptr<Projectile>> m_projectileRemovalQueue;

	std::vector<std::shared_ptr<EffectAttack>> m_effects;
	std::deque<std::shared_ptr<EffectAttack>> m_effectRemovalQueue;

	ProjectilePool m_projectilePool;
	EffectAttackPool m_effectPool; // 用于多线程保护
};

#endif //ATTACKMANAGER_HPP
