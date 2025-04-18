//
// Created by tjx20 on 3/26/2025.
//

#include "BulletManager.hpp"
#include <execution>
#include <thread>

#include "Scene/SceneManager.hpp"

void BulletManager::spawnBullet(const CharacterType type, const std::string& bulletImagePath, const Util::Transform& transform, glm::vec2 direction, float size, float speed, int damage, int numRebound) {
	const auto bullet = m_bulletPool.Acquire(type, bulletImagePath, transform, direction, size, speed, damage, numRebound);
	bullet->Init(); // 只初始化碰撞組件，不處理渲染
	// 加入渲染樹
	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	currentScene->GetRoot().lock()->AddChild(bullet);  // 由 BulletManager 的 shared_ptr 加入
	currentScene->GetCamera().lock()->AddChild(bullet);

	// 注冊到碰撞管理器
	if (auto currentRoom = currentScene->GetCurrentRoom())
	{
		currentRoom->GetCollisionManager()->RegisterNGameObject(bullet);
	}
	else
	{
		currentScene->GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION)->RegisterNGameObject(bullet);
	}
	// currentScene->GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION)->RegisterNGameObject(bullet);
	m_Bullets.push_back(bullet);
}

void BulletManager::Update() {
	if (m_Bullets.empty() && m_removalQueue.empty()) return;

	// 更新子彈位置
	for (auto& bullet : m_Bullets) {
		bullet->UpdateLocation(Util::Time::GetDeltaTimeMs());
	}

	// 過濾要移除的子彈
	std::vector<std::shared_ptr<Bullet>> activeBullets;
	activeBullets.reserve(m_Bullets.size());  // 預先分配空間
	for (auto& bullet : m_Bullets) {
		if (bullet->ShouldRemove()) {
			m_removalQueue.push_back(bullet);
			bullet->SetActive(false);
			bullet->SetVisible(false);
		} else {
			activeBullets.push_back(bullet);
		}
	}

	m_Bullets.swap(activeBullets);

	// 處理移除隊列
	constexpr size_t MAX_PROCESS_PER_FRAME = 5;
	constexpr int MAX_PROCESS_TIME_US = 1000;

	if (m_removalQueue.empty()) return;

	auto processStart = std::chrono::high_resolution_clock::now();
	auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!currentScene) return;

	auto root = currentScene->GetRoot().lock();
	auto camera = currentScene->GetCamera().lock();
	auto collisionManager = currentScene->GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION);

	if (!root || !camera || !collisionManager) return;

	size_t processedCount = 0;
	while (processedCount < MAX_PROCESS_PER_FRAME && !m_removalQueue.empty()) {
		auto bullet = std::move(m_removalQueue.front());
		m_removalQueue.pop_front();
		root->RemoveChild(bullet);
		camera->RemoveChild(bullet);
		collisionManager->UnregisterNGameObject(bullet);
		m_bulletPool.Release(bullet);
		processedCount++;

		auto now = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - processStart).count() > MAX_PROCESS_TIME_US)
			break;
	}
}