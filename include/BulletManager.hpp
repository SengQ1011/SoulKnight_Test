//
// Created by tjx20 on 3/26/2025.
//

#ifndef BULLETMANAGER_HPP
#define BULLETMANAGER_HPP

#include "Weapon/Bullet.hpp"
#include "BulletPool.hpp"
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
	void spawnBullet(const CharacterType type, const std::string& bulletImagePath,const Util::Transform& transform, glm::vec2 direction, float size, float speed, int damage, int numRebound);

private:
	std::vector<std::shared_ptr<Bullet>> m_Bullets;
	std::deque<std::shared_ptr<Bullet>> m_removalQueue;
	BulletPool m_bulletPool;
	std::mutex m_queueMutex; // 用于多线程保护
};

#endif //BULLETMANAGER_HPP
