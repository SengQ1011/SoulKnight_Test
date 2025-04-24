//
// Created by tjx20 on 4/5/2025.
//

#include "ObserveManager/TrackingManager.hpp"

#include "Components/AiComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/FollowerComponent.hpp"

#include "Creature/Character.hpp"
#include "Weapon/Weapon.hpp"

void TrackingManager::Update() {
	if (m_player.lock() == nullptr ||m_player.expired()) return;

	// 更新玩家數據
	m_playerPos = m_player.lock()->GetWorldCoord();

	// 更新有視野且最近敵人數據
	FindNearestVisibleEnemy();

	notifyObserver();
}

void TrackingManager::SetPlayer(const std::shared_ptr<Character> &player)
{
	if(player == nullptr) {
		for(auto &enemy: m_enemies) {
			if (const auto aiComp = enemy->GetComponent<AIComponent>(ComponentType::AI)) {
				aiComp->RemoveTarget();
				// LOG_DEBUG("removed");
			}
		}
	}
	m_player = player;
}

void TrackingManager::RemoveEnemy(const std::shared_ptr<Character> &enemy)
{
	// 通知玩家最近的enemy死掉了
	if (const auto attackComp = m_player.lock()->GetComponent<AttackComponent>(ComponentType::ATTACK)) {
		if (const auto followerComp = attackComp->GetCurrentWeapon()->GetComponent<FollowerComponent>(ComponentType::FOLLOWER)){
			followerComp->SetTarget(nullptr);
		}
	}
	m_enemies.erase(std::remove(m_enemies.begin(), m_enemies.end(), enemy), m_enemies.end());
}


// 射綫檢測：一條線段（從 rayStart 到 rayEnd）有沒有穿過一個矩形（rect）==》簡單 AABB 判斷
bool TrackingManager::RayIntersectsRect(const glm::vec2& rayStart, const glm::vec2& rayEnd, const Rect& rect) {
	// tmin:起點，tmax：終點
	float tmin = 0.0f;
	float tmax = 1.0f;
	// 起點到終點的方向向量
	glm::vec2 delta = rayEnd - rayStart;

	for (int i = 0; i < 2; ++i) {
		if (std::abs(delta[i]) < 1e-6f) {	// 1e-6f = 0.000001f 代表幾乎等於0
			if (rayStart[i] < rect.m_Position[i] || rayStart[i] > rect.m_Position[i] + rect.m_Size[i])
				return false;
		} else {
			// t1 = 線段穿過矩形「左/下邊界」的時候t的值
			// t2 = 穿過矩形「右上邊界」的時候t的值
			const float ood = 1.0f / delta[i];
			float t1 = (rect.m_Position[i] - rayStart[i]) * ood;
			float t2 = (rect.m_Position[i] + rect.m_Size[i] - rayStart[i]) * ood;
			if (t1 > t2) std::swap(t1, t2);
			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);
			if (tmin > tmax) return false;
		}
	}
	return true;
}

bool TrackingManager::HasLineOfSight(const glm::vec2& from, const glm::vec2& to) const {
	// 搜尋所有敵人、檢查有沒有 Terrain 層的障礙物擋住
	for (const auto& terrain : m_terrainObjects) {
		const auto collisionComp = terrain->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		if (!collisionComp) continue;
		Rect bounds = collisionComp->GetBounds();
		if (RayIntersectsRect(from, to, bounds)) {
			// LOG_DEBUG("TrackingManager::HasLineOfSight==>have terrain block");
			return false; // 有擋住
		}
	}
	return true;
}

void TrackingManager::FindNearestVisibleEnemy() {
	m_visibleEnemies.clear();		// 清空舊資料
	m_nearestVisibleEnemy.reset();
	float minDist = m_maxSightRange; // 直接使用视野范围作为初始值

	for (auto& enemy : m_enemies) {
		glm::vec2 enemyPos = enemy->GetWorldCoord();

		// 检测：距离 + 射线
		if (float dist = glm::distance(m_playerPos, enemyPos);
			dist <= m_maxSightRange){
			if (HasLineOfSight(m_playerPos, enemyPos)) {
				m_visibleEnemies.push_back(enemy);
				//LOG_DEBUG("have target");
				if (dist < minDist) {
					minDist = dist;
					m_nearestVisibleEnemy = enemy;
				}
			}
		}
	}
}

void TrackingManager::notifyObserver() {
	//TODO:修改（改爲使用父類的m_Observer）

	// 給玩家：只傳遞可見的最近敵人，若都沒有enemy視野就只通知一次玩家
	if (!m_visibleEnemies.empty()) {
		m_playerLostTarget = false;
		if (const auto attackComp = m_player.lock()->GetComponent<AttackComponent>(ComponentType::ATTACK)) {
			attackComp->OnEnemyPositionUpdate(m_nearestVisibleEnemy);
		}
	} else {
		if (!m_playerLostTarget) {
			m_playerLostTarget = true;
			if (const auto attackComp = m_player.lock()->GetComponent<AttackComponent>(ComponentType::ATTACK)) {
				attackComp->OnLostEnemy();// 失去目標
			}
		}
	}

	// 給敵人：分類型處理
	for (auto& enemy : m_enemies) {
		if (auto ai = enemy->GetComponent<AIComponent>(ComponentType::AI)) {
			// 只有當敵人在玩家視野內時才傳遞玩家位置
			if (std::find(m_visibleEnemies.begin(), m_visibleEnemies.end(), enemy) != m_visibleEnemies.end()) {
				ai->OnPlayerPositionUpdate(m_player);
			} else {
				ai->OnLostPlayer(); // 丟失目標的通知
			}
		}
	}
}