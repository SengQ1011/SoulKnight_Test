//
// Created by tjx20 on 5/9/2025.
//

// BlockProjectileStrategy.hpp
#ifndef BLOCKPROJECTILESTRATEGY_HPP
#define BLOCKPROJECTILESTRATEGY_HPP

#include "ITriggerStrategy.hpp"
#include "Attack/Projectile.hpp"

class BlockProjectileStrategy final : public ITriggerStrategy {
public:
	void OnTriggerEnter(std::shared_ptr<nGameObject> self,
						std::shared_ptr<nGameObject> other) override;
};

#endif // BLOCKPROJECTILESTRATEGY_HPP
