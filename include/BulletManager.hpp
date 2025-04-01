//
// Created by tjx20 on 3/26/2025.
//

#ifndef BULLETMANAGER_HPP
#define BULLETMANAGER_HPP

#include "Weapon/Bullet.hpp"
#include "Util/Time.hpp"

class BulletManager {
public:
	// 构造函数与析构函数
	BulletManager() = default;
	~BulletManager() = default;

	// 禁止拷贝与赋值
	BulletManager(const BulletManager&) = delete;
	BulletManager& operator=(const BulletManager&) = delete;

	// 更新所有子弹
	void Update();
	const std::vector<std::shared_ptr<Bullet>>& GetBullets() const {return m_Bullets;}
	void spawnBullet(const CharacterType type, const std::string& bulletImagePath,const Util::Transform& transform, glm::vec2 direction, float size, float speed, int damage);

private:
	std::vector<std::shared_ptr<Bullet>> m_Bullets;
};

#endif //BULLETMANAGER_HPP
