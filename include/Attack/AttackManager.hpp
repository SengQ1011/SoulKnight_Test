//
// Created by tjx20 on 3/26/2025.
//

#ifndef ATTACKMANAGER_HPP
#define ATTACKMANAGER_HPP

#include "Attack/EffectAttackPool.hpp"
#include "Attack/ProjectilePool.hpp"
#include "ObserveManager/IManager.hpp"

class AttackManager : public IManager{
public:
	// 构造函数与析构函数
	AttackManager() = default;
	~AttackManager() override= default;

	// 禁止拷贝与赋值
	AttackManager(const AttackManager&) = delete;
	AttackManager& operator=(const AttackManager&) = delete;

	// 更新所有攻擊物件
	void Update() override;

	//----Getter----
	const std::vector<std::shared_ptr<Projectile>>& GetProjectiles() const { return m_projectiles; }
	const std::vector<std::shared_ptr<EffectAttack>>& GetEffects() const { return m_effects; }

	// Bubble延緩加入->否則一邊更新一邊加入vector，會導致迭代器/引用失效
	void AttackManager::EnqueueSpawn(std::function<void()> spawnFunc) {
		m_spawnQueue.push_back(std::move(spawnFunc));
	}
	// 射出子彈類 OR 斬擊動畫
	void spawnProjectile(const CharacterType type, const Util::Transform& transform, glm::vec2 direction, float size, int damage, const std::string& bulletImagePath, float speed,
		int numRebound, bool canReboundBySword, bool isBubble, bool bubbleTrail, const std::string &bubbleImagePath);
	void spawnEffectAttack(const CharacterType type, const Util::Transform& transform, glm::vec2 direction, float size, int damage, bool canReflect, EffectAttackType effectType);


private:
	std::vector<std::function<void()>> m_spawnQueue;		// 收集這回合更新時新增的子彈(bubble)
	std::vector<std::shared_ptr<Projectile>> m_projectiles;
	std::deque<std::shared_ptr<Projectile>> m_projectileRemovalQueue;

	std::vector<std::shared_ptr<EffectAttack>> m_effects;
	std::deque<std::shared_ptr<EffectAttack>> m_effectRemovalQueue;

	ProjectilePool m_projectilePool;
	EffectAttackPool m_effectPool; // 用于多线程保护
};

#endif //ATTACKMANAGER_HPP
