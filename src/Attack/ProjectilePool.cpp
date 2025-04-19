//
// Created by tjx20 on 4/18/2025.
//

#include "Attack/ProjectilePool.hpp"

std::deque<std::shared_ptr<Projectile>> ProjectilePool::pool;

std::shared_ptr<Projectile> ProjectilePool::Acquire(const CharacterType type, const std::string& ImagePath, const Util::Transform &bulletTransform,
								const glm::vec2 direction, const float size, const int damage, const float speed, const int numRebound) {
	if (!pool.empty()) {
		// 從池中取出子彈並重置屬性
		auto bullet = pool.back();
		if (!bullet) {
			//LOG_ERROR("bullet from pool is nullptr!");
		}
		bullet->ResetAll(type, bulletTransform, direction, size, damage, ImagePath, speed, numRebound);
		pool.pop_back();  // 從池中移除
		bullet->SetActive(true);   // 激活子彈
		bullet->SetVisible(true);
		//LOG_DEBUG("use old bullet");
		return bullet;
	}

	// 如果池是空的，創建一個新的子彈
	auto newBullet = std::make_shared<Projectile>(type, bulletTransform, direction, size, damage, ImagePath, speed, numRebound);
	LOG_DEBUG("created new bullet");
	return newBullet;
}