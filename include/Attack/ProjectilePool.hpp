//
// Created by tjx20 on 4/13/2025.
//

#ifndef PROJECTILEPOOL_HPP
#define PROJECTILEPOOL_HPP

#include <deque>

#include "Attack/Projectile.hpp"

class ProjectilePool {
public:
	std::shared_ptr<Projectile> Acquire(const CharacterType type, const std::string& ImagePath, const Util::Transform &bulletTransform,
								const glm::vec2 direction, const float size, const int damage, const float speed, const int numRebound);

	void Release(const std::shared_ptr<Projectile>& bullet) {
		pool.push_back(std::move(bullet));
	}

private:
	static std::deque<std::shared_ptr<Projectile>> pool; // 更高效的内存管理
};


#endif //PROJECTILEPOOL_HPP
