//
// Created by tjx20 on 4/5/2025.
//

#ifndef TRACKINGMANAGER_HPP
#define TRACKINGMANAGER_HPP

#include "ObserveManager/ObserveManager.hpp"

class TrackingManager : public ObserveManager {
	void Update() {
		UpdatePlayerTarget(player);

		for (auto& enemy : m_enemies) {
			// 敌人根据玩家的坐标做出相应的动作
			UpdateEnemyTarget(enemy);
		}
	}

	void notifyObserver() override;
private:
	std::shared_ptr<Player> m_players;
	std::vector<std::shared_ptr<Enemy>> m_enemies;
};

#endif //TRACKINGMANAGER_HPP
