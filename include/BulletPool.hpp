//
// Created by tjx20 on 4/13/2025.
//

#ifndef BULLETPOOL_HPP
#define BULLETPOOL_HPP

#include <deque>

#include "Weapon/Bullet.hpp"

class BulletPool {
public:
	std::shared_ptr<Bullet> Acquire(const CharacterType type, const std::string& ImagePath, const Util::Transform &bulletTransform,
								const glm::vec2 direction, const float size, const float speed, const int damage, const int numRebound);

	void Release(const std::shared_ptr<Bullet>& bullet) {
		pool.push_back(std::move(bullet));
	}

private:
	static std::deque<std::shared_ptr<Bullet>> pool; // 更高效的内存管理
};


#endif //BULLETPOOL_HPP
