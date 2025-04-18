//
// Created by tjx20 on 3/26/2025.
//

#include "BulletManager.hpp"
#include <execution>
#include <thread>

#include "Scene/SceneManager.hpp"

void BulletManager::spawnBullet(const CharacterType type, const std::string& bulletImagePath, const Util::Transform& transform, glm::vec2 direction, float size, float speed, int damage, int numRebound) {
	const auto bullet = m_bulletPool.Acquire(type, bulletImagePath, transform, direction, size, speed, damage, numRebound);
	bullet->PostInitialize(); // 只初始化碰撞組件，不處理渲染
	// 加入渲染樹
	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	currentScene->GetRoot().lock()->AddChild(bullet);  // 由 BulletManager 的 shared_ptr 加入
	currentScene->GetCamera().lock()->AddChild(bullet);

	// 注冊到碰撞管理器
	currentScene->GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION)->RegisterNGameObject(bullet);

	m_Bullets.push_back(bullet);
}

void BulletManager::Update() {
	if (m_Bullets.empty() && m_removalQueue.empty()) return;

	LOG_DEBUG("Bullet ========> {}", m_Bullets.size());
	const auto updateStart = std::chrono::high_resolution_clock::now();

	// 1. 更新子彈位置
	for (auto& bullet : m_Bullets) {
		bullet->UpdateLocation(Util::Time::GetDeltaTimeMs());
	}
	const auto updateEnd = std::chrono::high_resolution_clock::now();
	//LOG_DEBUG("update Location time: {}", std::chrono::duration_cast<std::chrono::microseconds>(updateEnd - updateStart).count());

	// 2. 過濾要移除的子彈
	std::vector<std::shared_ptr<Bullet>> activeBullets;
	activeBullets.reserve(m_Bullets.size());  // 預先分配空間
	for (auto& bullet : m_Bullets) {
		if (bullet->ShouldRemove()) {
			m_removalQueue.push_back(bullet);
			bullet->SetActive(false);
			// auto t1 = std::chrono::high_resolution_clock::now();
			bullet->SetVisible(false);
			// auto t2 = std::chrono::high_resolution_clock::now();
			// LOG_DEBUG("SetVisible took {}us", (t2 - t1).count());
		} else {
			activeBullets.push_back(bullet);
		}
	}
	const auto markTime = std::chrono::high_resolution_clock::now();
	LOG_DEBUG("Mark time: {}", std::chrono::duration_cast<std::chrono::microseconds>(markTime - updateEnd).count());
	if(!activeBullets.empty()) m_Bullets.swap(activeBullets);
	const auto swapTime = std::chrono::high_resolution_clock::now();
	LOG_DEBUG("swap time: {}", std::chrono::duration_cast<std::chrono::microseconds>(swapTime - markTime).count());

	// 3. 處理移除隊列
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

	auto check1 = std::chrono::high_resolution_clock::now();
	// LOG_DEBUG("Get root: {}us", std::chrono::duration_cast<std::chrono::microseconds>(check1 - processStart).count());

	size_t processedCount = 0;
	while (processedCount < MAX_PROCESS_PER_FRAME && !m_removalQueue.empty()) {
		auto bullet = std::move(m_removalQueue.front());
		m_removalQueue.pop_front();

		auto t1 = std::chrono::high_resolution_clock::now();
		root->RemoveChild(bullet);
		auto t2 = std::chrono::high_resolution_clock::now();
		LOG_DEBUG("until RemoveChild root: {}us", std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());
		camera->RemoveChild(bullet);
		auto t3 = std::chrono::high_resolution_clock::now();
		LOG_DEBUG("until RemoveChild camera: {}us", std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count());
		collisionManager->UnregisterNGameObject(bullet);
		auto t4 = std::chrono::high_resolution_clock::now();
		LOG_DEBUG("until Unregister: {}us", std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count());
		m_bulletPool.Release(bullet);
		auto t5 = std::chrono::high_resolution_clock::now();
		LOG_DEBUG("Release: {}us", std::chrono::duration_cast<std::chrono::microseconds>(t5 - t4).count());

		processedCount++;
		auto now = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::microseconds>(now - processStart).count() > MAX_PROCESS_TIME_US)
			break;
	}

	auto time = std::chrono::high_resolution_clock::now();
	LOG_DEBUG("All Release&unregister: {}us", std::chrono::duration_cast<std::chrono::microseconds>(time - check1).count());

	auto totalEnd = std::chrono::high_resolution_clock::now();
	LOG_DEBUG("Elapsed Total: {}us", std::chrono::duration_cast<std::chrono::microseconds>(totalEnd - updateStart).count());
}