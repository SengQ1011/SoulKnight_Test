//
// Created by tjx20 on 3/26/2025.
//

#include "BulletManager.hpp"

BulletManager& BulletManager::GetInstance() {
	static BulletManager instance;
	return instance;
}

void BulletManager::spawnBullet(const std::string& bulletImagePath, const Util::Transform& transform, glm::vec2 direction, float speed, int damage) {
	bullets.push_back(std::make_shared<Bullet>(bulletImagePath, transform, direction, speed, damage));
}

void BulletManager::Update() {
	if (bullets.empty()) return;
	for (auto it = bullets.begin(); it != bullets.end();) {
		(*it)->Update();
		if ((*it)->isOutOfBounds()) {
			it = bullets.erase(it);
		} else {
			++it;
		}
	}
}