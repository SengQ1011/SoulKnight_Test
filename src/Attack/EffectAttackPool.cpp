//
// Created by tjx20 on 4/19/2025.
//

#include "Attack/EffectAttackPool.hpp"

std::deque<std::shared_ptr<EffectAttack>> EffectAttackPool::pool;

EffectAttackPool::EffectAttackPool()
{
	for (int i=0; i < 60; i++)
	{
		EffectAttackInfo effect_attack;
		effect_attack.type = CharacterType::ENEMY;
		effect_attack.size = 20.0f;
		effect_attack.damage = 0;
		effect_attack.elementalDamage = StatusEffect::NONE;
		effect_attack.chainAttack.enabled = false;
		effect_attack.canBlockingBullet = false;
		effect_attack.canReflectBullet = false;
		effect_attack.effectType = EffectAttackType::SLASH;
		auto effect = std::make_shared<EffectAttack>(effect_attack);
		pool.push_back(effect);
	}
}

std::shared_ptr<EffectAttack> EffectAttackPool::Acquire(const EffectAttackInfo &effectAttackInfo) {
	if (!pool.empty()) {
		// 從池中取出子彈並重置屬性
		auto effectAttack = pool.back();
		if (!effectAttack) LOG_ERROR("effectAttack = nullptr");
		effectAttack->ResetAll(effectAttackInfo);
		pool.pop_back();  // 從池中移除
		effectAttack->SetActive(true);   // 激活子彈
		effectAttack->SetControlVisible(true);
		return effectAttack;
	}
	// 如果池是空的，創建一個新的子彈
	auto newEffectAttack = std::make_shared<EffectAttack>(effectAttackInfo);
	return newEffectAttack;
}