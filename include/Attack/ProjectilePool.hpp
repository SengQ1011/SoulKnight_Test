//
// Created by tjx20 on 4/13/2025.
//

#ifndef PROJECTILEPOOL_HPP
#define PROJECTILEPOOL_HPP

#include <deque>
#include "Attack/Projectile.hpp"

class ProjectilePool {
public:
	explicit ProjectilePool();
	~ProjectilePool() = default;
	std::shared_ptr<Projectile> Acquire(const ProjectileInfo& projectileInfo);

	void Release(const std::shared_ptr<Projectile>& bullet) {
		pool.push_back(std::move(bullet));
	}

private:
	static std::deque<std::shared_ptr<Projectile>> pool; // 更高效的内存管理
};


#endif //PROJECTILEPOOL_HPP
