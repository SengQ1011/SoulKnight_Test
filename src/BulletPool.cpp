//
// Created by tjx20 on 4/18/2025.
//

#include "BulletPool.hpp"

std::deque<std::shared_ptr<Bullet>> BulletPool::pool;

std::shared_ptr<Bullet> BulletPool::Acquire(const CharacterType type, const std::string& ImagePath, const Util::Transform &bulletTransform,
								const glm::vec2 direction, const float size, const float speed, const int damage, const int numRebound) {
	if (!pool.empty()) {
		// 從池中取出子彈並重置屬性
		auto bullet = pool.back();
		if (!bullet) {
			LOG_ERROR("bullet from pool is nullptr!");
		}
		bullet->ResetAll(type, ImagePath, bulletTransform, direction, size, speed, damage, numRebound);
		pool.pop_back();  // 從池中移除
		bullet->SetActive(true);   // 激活子彈
		bullet->SetVisible(true);
		return bullet;
	}

	// 如果池是空的，創建一個新的子彈
	auto newBullet = std::make_shared<Bullet>(type, ImagePath, bulletTransform, direction, size, speed, damage, numRebound);
	return newBullet;
}