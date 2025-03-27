//
// Created by tjx20 on 3/26/2025.
//

#ifndef BULLETMANAGER_HPP
#define BULLETMANAGER_HPP

#include "Weapon/Bullet.hpp"
#include "Util/Time.hpp"

class BulletManager {
public:
	// 單例模式 (Singleton Pattern)：取得唯一的BulletManager
	static BulletManager& GetInstance();

	// 禁止拷貝與賦值，確保只有一個實例
	BulletManager(const BulletManager&) = delete;
	BulletManager& operator=(const BulletManager&) = delete;

	void Update();
	const std::vector<std::shared_ptr<Bullet>>& GetBullets() const {return bullets;}
	void spawnBullet(const std::string& bulletImagePath,const Util::Transform& transform, glm::vec2 direction, float speed, int damage);

private:
	BulletManager() = default;
	std::vector<std::shared_ptr<Bullet>> bullets;
};

#endif //BULLETMANAGER_HPP
