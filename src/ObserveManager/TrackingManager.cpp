//
// Created by tjx20 on 4/5/2025.
//

#include "ObserveManager/TrackingManager.hpp"

#include "Components/AiComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/FlickerComponent.hpp"
#include "Components/FollowerComponent.hpp"


#include "Creature/Character.hpp"
#include "Weapon/Weapon.hpp"

void TrackingManager::Update()
{
	if (m_player.lock() == nullptr || m_player.expired())
		return;

	// 更新玩家數據
	m_playerPos = m_player.lock()->GetWorldCoord();

	// 更新有視野且最近敵人數據
	FindNearestVisibleEnemy();

	notifyObserver();
}

void TrackingManager::SetPlayer(const std::shared_ptr<Character> &player)
{
	if (player == nullptr)
	{
		for (auto &enemy : m_enemies)
		{
			if (const auto attackComp = enemy->GetComponent<AttackComponent>(ComponentType::ATTACK))
			{
				attackComp->OnLostTarget();
			}
			if (const auto aiComp = enemy->GetComponent<AIComponent>(ComponentType::AI))
			{
				aiComp->RemoveTarget();
			}
		}
	}
	m_player = player;
}

void TrackingManager::RemoveEnemy(const std::shared_ptr<Character> &enemy)
{
	// 通知玩家最近的enemy死掉了
	if (const auto attackComp = m_player.lock()->GetComponent<AttackComponent>(ComponentType::ATTACK))
	{
		if (const auto followerComp =
				attackComp->GetCurrentWeapon()->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
		{
			followerComp->SetTarget(nullptr);
		}
	}
	m_enemies.erase(std::remove(m_enemies.begin(), m_enemies.end(), enemy), m_enemies.end());
}

void TrackingManager::RemoveTerrainObject(const std::shared_ptr<nGameObject> &terrain)
{
	m_terrainObjects.erase(std::remove(m_terrainObjects.begin(), m_terrainObjects.end(), terrain),
						   m_terrainObjects.end());
}

void TrackingManager::RemoveTerrainObjects(const std::vector<std::shared_ptr<nGameObject>> &terrains)
{
	for (const auto &terrain : terrains)
	{
		RemoveTerrainObject(terrain);
	}
}

// 射綫檢測：一條線段（從 rayStart 到 rayEnd）有沒有穿過一個矩形（rect）==》簡單 AABB 判斷
bool TrackingManager::RayIntersectsRect(const glm::vec2 &rayStart, const glm::vec2 &rayEnd, const Rect &rect)
{
	// tmin:起點，tmax：終點
	float tmin = 0.0f;
	float tmax = 1.0f;
	// 起點到終點的方向向量
	glm::vec2 delta = rayEnd - rayStart;

	for (int i = 0; i < 2; ++i)
	{
		// 正確獲取矩形的邊界
		float minBound, maxBound;
		if (i == 0)
		{ // X軸
			minBound = rect.left();
			maxBound = rect.right();
		}
		else
		{ // Y軸
			minBound = rect.bottom();
			maxBound = rect.top();
		}

		if (std::abs(delta[i]) < 1e-6f)
		{ // 1e-6f = 0.000001f 代表幾乎等於0
			if (rayStart[i] < minBound || rayStart[i] > maxBound)
				return false;
		}
		else
		{
			// t1 = 線段穿過矩形邊界的時候t的值
			const float ood = 1.0f / delta[i];
			float t1 = (minBound - rayStart[i]) * ood;
			float t2 = (maxBound - rayStart[i]) * ood;
			if (t1 > t2)
				std::swap(t1, t2);
			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);
			if (tmin > tmax)
				return false;
		}
	}
	return true;
}

bool TrackingManager::HasLineOfSight(const glm::vec2 &from, const glm::vec2 &to) const
{
	// 搜尋所有敵人、檢查有沒有 Terrain 層的障礙物擋住
	for (const auto &terrain : m_terrainObjects)
	{
		const auto collisionComp = terrain->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		if (!collisionComp)
			continue;
		Rect bounds = collisionComp->GetBounds();
		if (RayIntersectsRect(from, to, bounds))
		{
			return false; // 有擋住
		}
	}
	return true;
}

void TrackingManager::FindNearestVisibleEnemy()
{
	m_visibleEnemies.clear(); // 清空舊資料
	m_nearestVisibleEnemy.reset();
	float minDist = m_maxSightRange; // 直接使用视野范围作为初始值

	for (auto &enemy : m_enemies)
	{
		// 檢查敵人是否應該被追蹤（排除閃爍情況）
		if (!ShouldTrackEnemy(enemy))
			continue;
		glm::vec2 enemyPos = enemy->GetWorldCoord();

		// 检测：距离 + 射线
		if (float dist = glm::distance(m_playerPos, enemyPos); dist <= m_maxSightRange)
		{
			if (HasLineOfSight(m_playerPos, enemyPos))
			{
				m_visibleEnemies.push_back(enemy);
				if (dist < minDist)
				{
					minDist = dist;
					m_nearestVisibleEnemy = enemy;
				}
			}
		}
	}
}

bool TrackingManager::ShouldTrackEnemy(const std::shared_ptr<Character> &enemy) const
{
	// 首先檢查基本的激活狀態
	if (!enemy->IsActive())
		return false;

	// 如果敵人不可見，檢查是否是因為閃爍
	if (!enemy->IsControlVisible())
	{
		// 檢查是否有閃爍組件並且正在閃爍
		if (auto flickerComp = enemy->GetComponent<FlickerComponent>(ComponentType::FLICKER))
		{
			// 如果正在閃爍，仍然應該被追蹤
			return flickerComp->IsFlickering();
		}
		// 如果不是閃爍造成的不可見，則不應該追蹤
		return false;
	}

	// 可見且激活的敵人應該被追蹤
	return true;
}

void TrackingManager::notifyObserver()
{
	// TODO:修改（改爲使用父類的m_Observer）

	// 給玩家：只傳遞可見的最近敵人，若都沒有enemy視野就只通知一次玩家
	if (!m_visibleEnemies.empty())
	{
		m_playerLostTarget = false;
		if (const auto attackComp = m_player.lock()->GetComponent<AttackComponent>(ComponentType::ATTACK))
		{
			attackComp->OnTargetPositionUpdate(m_nearestVisibleEnemy);
		}
	}
	else
	{
		if (!m_playerLostTarget)
		{
			m_playerLostTarget = true;
			if (const auto attackComp = m_player.lock()->GetComponent<AttackComponent>(ComponentType::ATTACK))
			{
				attackComp->OnLostTarget(); // 失去目標
			}
		}
	}

	// 給敵人：分類型處理
	for (auto &enemy : m_enemies)
	{
		if (auto ai = enemy->GetComponent<AIComponent>(ComponentType::AI))
		{
			// 只有當敵人在玩家視野內時才傳遞玩家位置
			if (std::find(m_visibleEnemies.begin(), m_visibleEnemies.end(), enemy) != m_visibleEnemies.end())
			{
				ai->OnPlayerPositionUpdate(m_player);
			}
			else
			{
				ai->OnLostPlayer(); // 丟失目標的通知
			}
		}
	}
}
