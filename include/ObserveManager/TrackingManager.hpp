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

	//----Getter----
	[[nodiscard]] std::vector<std::shared_ptr<Character>> GetEnemies() const { return m_enemies;}

	//----Setter----
    void SetPlayer(const std::shared_ptr<Character>& player){
    	if(player == nullptr) {
    		for(auto &enemy: m_enemies) {
    			if (const auto aiComp = enemy->GetComponent<AIComponent>(ComponentType::AI)) {
    				aiComp->RemoveTarget();
    				LOG_DEBUG("removed");
    			}
    		}
    	}
	    m_player = player;
    }
    void AddEnemy(const std::shared_ptr<Character>& enemy) { m_enemies.push_back(enemy); }
    void RemoveEnemy(const std::shared_ptr<Character>& enemy) {
    	// 通知玩家最近的enemy死掉了
		if (const auto attackComp = m_player.lock()->GetComponent<AttackComponent>(ComponentType::ATTACK)) {
			if (const auto followerComp = attackComp->GetCurrentWeapon()->GetComponent<FollowerComponent>(ComponentType::FOLLOWER)){
				followerComp->SetTarget(nullptr);
			}
		}
	    m_enemies.erase(std::remove(m_enemies.begin(), m_enemies.end(), enemy), m_enemies.end());
    }
	void AddTerrainObjects(const std::vector<std::shared_ptr<nGameObject>>& terrains){
		for(const auto& terrain : terrains ) {
			m_terrainObjects.push_back(terrain);
		}
    }

	// 視野檢測接口
	bool HasLineOfSight(const glm::vec2& from, const glm::vec2& to)const;
	static bool RayIntersectsRect(const glm::vec2& rayStart, const glm::vec2& rayEnd, const Rect& rect) ;
private:
	void FindNearestVisibleEnemy();

    void notifyObserver() override;

    std::weak_ptr<Character> m_player;
	std::weak_ptr<Character> m_nearestVisibleEnemy;
	glm::vec2 m_playerPos;
    std::vector<std::shared_ptr<Character>> m_enemies;
	std::vector<std::shared_ptr<Character>> m_visibleEnemies;
	std::vector<std::shared_ptr<nGameObject>> m_terrainObjects;
    float m_maxSightRange = 1000.0f; // 無視障礙物的強制檢測範圍
};

#endif //TRACKINGMANAGER_HPP
