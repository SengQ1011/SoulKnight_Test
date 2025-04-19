//
// Created by tjx20 on 3/26/2025.
//

#include "Attack/AttackManager.hpp"
#include <execution>
#include <thread>

#include "Room/RoomCollisionManager.hpp"
#include "Scene/SceneManager.hpp"

void AttackManager::spawnProjectile(const CharacterType type, const Util::Transform& transform, glm::vec2 direction, float size, int damage, const std::string& bulletImagePath, float speed, int numRebound)
{
	const auto bullet = m_projectilePool.Acquire(type, bulletImagePath, transform, direction, size, damage, speed, numRebound);
	if (bullet == nullptr) {LOG_ERROR("bullet from pool is nullptr!");}
	bullet->Init(); // 只初始化碰撞組件，不處理渲染

	// 加入渲染樹
	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	currentScene->GetRoot().lock()->AddChild(bullet);  // 由 AttackManager 的 shared_ptr 加入
	currentScene->GetCamera().lock()->AddChild(bullet);

	// 注冊到碰撞管理器
	const std::shared_ptr<RoomCollisionManager> collisionManager = currentScene->GetCurrentCollisionManager();
	collisionManager->RegisterNGameObject(bullet);
	m_projectiles.push_back(bullet);
}

void AttackManager::spawnEffectAttack(const CharacterType type, const Util::Transform& transform, glm::vec2 direction, float size, int damage, bool canReflect, EffectAttackType effectType) {
	auto effectAttack = m_effectPool.Acquire(type, transform, direction, size, damage, canReflect, effectType);
	effectAttack->Init();

	// 加入渲染樹
	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	currentScene->GetRoot().lock()->AddChild(effectAttack);  // 由 AttackManager 的 shared_ptr 加入
	currentScene->GetCamera().lock()->AddChild(effectAttack);

	// 注冊到碰撞管理器
	const std::shared_ptr<RoomCollisionManager> collisionManager = currentScene->GetCurrentCollisionManager();
	collisionManager->RegisterNGameObject(effectAttack);

	m_effects.push_back(effectAttack);
}

void AttackManager::Update() {
    if (m_projectiles.empty() && m_projectileRemovalQueue.empty() &&
        m_effects.empty() && m_effectRemovalQueue.empty()) {
    	return;
    }

    const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

    // 更新子彈與特效
    for (auto& bullet : m_projectiles) {
        bullet->UpdateObject(deltaTime);
    }
    for (auto& effect : m_effects) {
        effect->UpdateObject(deltaTime);
    }

    // 處理要移除的子彈
    {
        std::vector<std::shared_ptr<Projectile>> activeBullets;
        activeBullets.reserve(m_projectiles.size());

        for (auto& bullet : m_projectiles) {
            if (bullet->ShouldRemove()) {
                bullet->SetActive(false);
                bullet->SetVisible(false);
                m_projectileRemovalQueue.push_back(bullet);
            } else {
                activeBullets.push_back(bullet);
            }
        }

        m_projectiles.swap(activeBullets); // 移除掉無效的子彈
    }

    // 處理要移除的動畫特效
    {
        std::vector<std::shared_ptr<EffectAttack>> activeEffects;
        activeEffects.reserve(m_effects.size());

        for (auto& effect : m_effects) {
            if (effect->ShouldRemove()) {
                effect->SetActive(false);
                effect->SetVisible(false);
                m_effectRemovalQueue.push_back(effect);
            } else {
                activeEffects.push_back(effect);
            }
        }

        m_effects.swap(activeEffects);
    }

    // 處理移除隊列（子彈 + 特效）
    constexpr size_t MAX_PROCESS_PER_FRAME = 3;
    constexpr int MAX_PROCESS_TIME_US = 1000;

    auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
    if (!currentScene) return;

    auto root = currentScene->GetRoot().lock();
    auto camera = currentScene->GetCamera().lock();
    const std::shared_ptr<RoomCollisionManager> collisionManager = currentScene->GetCurrentCollisionManager();

    if (!root || !camera || !collisionManager) return;

    auto processStart = std::chrono::high_resolution_clock::now();
    size_t processedCount = 0;

    while (processedCount < MAX_PROCESS_PER_FRAME && !m_projectileRemovalQueue.empty()) {
        auto bullet = std::move(m_projectileRemovalQueue.front());
        m_projectileRemovalQueue.pop_front();

        root->RemoveChild(bullet);
        camera->RemoveChild(bullet);
        collisionManager->UnregisterNGameObject(bullet);
        m_projectilePool.Release(bullet);
        processedCount++;

        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::microseconds>(now - processStart).count() > MAX_PROCESS_TIME_US)
            break;
    }

    processedCount = 0;
    while (processedCount < MAX_PROCESS_PER_FRAME && !m_effectRemovalQueue.empty()) {
        auto effect = std::move(m_effectRemovalQueue.front());
        m_effectRemovalQueue.pop_front();

        root->RemoveChild(effect);
        camera->RemoveChild(effect);
        m_effectPool.Release(effect);
        processedCount++;

        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::microseconds>(now - processStart).count() > MAX_PROCESS_TIME_US)
            break;
    }
}