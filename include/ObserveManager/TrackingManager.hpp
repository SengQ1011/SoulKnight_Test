//
// Created by tjx20 on 4/5/2025.
//

#ifndef TRACKINGMANAGER_HPP
#define TRACKINGMANAGER_HPP

#include "Creature/Character.hpp"
#include "Components/AiComponent.hpp"
#include "Components/FollowerComponent.hpp"
#include "Components/CollisionComponent.hpp"
#include "ObserveManager/ObserveManager.hpp"

class TrackingManager : public ObserveManager {
public:
    void Update() override;

	//----Setter----
    void SetPlayer(std::shared_ptr<Character> player) { m_player = player; }
    void AddEnemy(std::shared_ptr<Character> enemy) { m_enemies.push_back(enemy); }
    void RemoveEnemy(std::shared_ptr<Character> enemy) { m_enemies.erase(std::remove(m_enemies.begin(), m_enemies.end(), enemy), m_enemies.end()); }

	// 視野檢測接口
	bool HasLineOfSight(const glm::vec2& from, const glm::vec2& to)const;
	bool RayIntersectsRect(const glm::vec2& rayStart, const glm::vec2& rayEnd, const Rect& rect) const;
private:
	void FindNearestVisibleEnemy();

    void notifyObserver() override;

    std::weak_ptr<Character> m_player;
	std::weak_ptr<Character> m_nearestVisibleEnemy;
	glm::vec2 m_playerPos;
    std::vector<std::shared_ptr<Character>> m_enemies;
	std::vector<std::shared_ptr<Character>> m_visibleEnemies;
    float m_maxSightRange = 100.0f; // 無視障礙物的強制檢測範圍
};

#endif //TRACKINGMANAGER_HPP
