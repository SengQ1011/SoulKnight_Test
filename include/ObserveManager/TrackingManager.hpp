//
// Created by tjx20 on 4/5/2025.
//

#ifndef TRACKINGMANAGER_HPP
#define TRACKINGMANAGER_HPP

#include "ObserveManager/ObserveManager.hpp"
#include "Structs/CollisionComponentStruct.hpp"

class nGameObject;
class Character;

class TrackingManager : public ObserveManager {
public:
    void Update() override;

	//----Getter----
	[[nodiscard]] std::vector<std::shared_ptr<Character>> GetEnemies() const { return m_enemies;}

	//----Setter----
    void SetPlayer(const std::shared_ptr<Character>& player);
    void AddEnemy(const std::shared_ptr<Character>& enemy) { m_enemies.push_back(enemy); }
    void RemoveEnemy(const std::shared_ptr<Character>& enemy);

	void AddTerrainObject(const std::shared_ptr<nGameObject>& terrain){ m_terrainObjects.push_back(terrain);}
	void AddTerrainObjects(const std::vector<std::shared_ptr<nGameObject>>& terrains){
		for(const auto& terrain : terrains ) {
			m_terrainObjects.push_back(terrain);
		}
    }

	// 視野檢測接口
	bool HasLineOfSight(const glm::vec2& from, const glm::vec2& to)const;
	static bool RayIntersectsRect(const glm::vec2 &rayStart, const glm::vec2 &rayEnd, const Rect &rect) ;
private:
	void FindNearestVisibleEnemy();

    void notifyObserver() override;

    std::weak_ptr<Character> m_player;
	std::weak_ptr<Character> m_nearestVisibleEnemy;
	glm::vec2 m_playerPos;
    std::vector<std::shared_ptr<Character>> m_enemies;
	std::vector<std::shared_ptr<Character>> m_visibleEnemies;
	std::vector<std::shared_ptr<nGameObject>> m_terrainObjects;
    float m_maxSightRange = 350.0f; // 無視障礙物的強制檢測範圍
	bool m_playerLostTarget = false;
};

#endif //TRACKINGMANAGER_HPP
