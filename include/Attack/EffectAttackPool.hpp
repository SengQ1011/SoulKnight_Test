//
// Created by tjx20 on 4/19/2025.
//

#ifndef EFFECTATTACKPOOL_HPP
#define EFFECTATTACKPOOL_HPP

#include <deque>
#include "Attack/EffectAttack.hpp"

class EffectAttackPool {
public:
	std::shared_ptr<EffectAttack> Acquire(const EffectAttackInfo &effect_attack_info);

	void Release(const std::shared_ptr<EffectAttack> &effectAttack) { pool.push_back(std::move(effectAttack)); }

private:
	static std::deque<std::shared_ptr<EffectAttack>> pool;
};

#endif //EFFECTATTACKPOOL_HPP
