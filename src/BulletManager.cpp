//
// Created by tjx20 on 3/26/2025.
//

#include "BulletManager.hpp"
#include <execution>
#include "Scene/SceneManager.hpp"

void BulletManager::spawnBullet(const std::string& bulletImagePath, const Util::Transform& transform, glm::vec2 direction, float speed, int damage) {
	auto bullet = std::make_shared<Bullet>(bulletImagePath, transform, direction, speed, damage);
	bullet->PostInitialize(); // 只初始化碰撞組件，不處理渲染
	// 加入渲染樹
	auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	currentScene->GetRoot().lock()->AddChild(bullet);  // 由 BulletManager 的 shared_ptr 加入
	currentScene->GetCamera().lock()->AddRelativePivotChild(bullet);

	// TODO:注冊到碰撞管理器
	currentScene->GetManager<RoomCollisionManager>("RoomCollisionManager")->RegisterNGameObject(bullet);

	m_Bullets.push_back(bullet);
}

void BulletManager::Update() {
	float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	if (m_Bullets.empty()) return;
	// 並行更新
	if (m_Bullets.size() > 100) {  // 仅在大数据时使用并行
		std::for_each(std::execution::par, m_Bullets.begin(), m_Bullets.end(),
			[deltaTime](auto& bullet) { bullet->UpdateLocation(deltaTime); });
	} else {
		std::for_each(std::execution::seq, m_Bullets.begin(), m_Bullets.end(),
			[deltaTime](auto& bullet) { bullet->UpdateLocation(deltaTime); });
	}

	// 移除碰撞的子彈
	auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	m_Bullets.erase(std::remove_if(m_Bullets.begin(), m_Bullets.end(),
		[currentScene](const auto& bullet) {
			if (bullet->ShouldRemove()) {
				currentScene->GetRoot().lock()->RemoveChild(bullet);
				currentScene->GetCamera().lock()->RemoveRelativePivotChild(bullet);
				currentScene->GetManager<RoomCollisionManager>("RoomCollisionManager")->UnregisterNGameObject(bullet);
				return true;
			}
			return false;
		}),
	m_Bullets.end());
}