//
// Created by tjx20 on 4/19/2025.
//

#include "Attack/EffectAttackPool.hpp"


std::deque<std::shared_ptr<EffectAttack>> EffectAttackPool::pool;

std::shared_ptr<EffectAttack> EffectAttackPool::Acquire(const CharacterType type, const Util::Transform &bulletTransform,
								const glm::vec2 direction, const float size, const int damage, bool canReflect,EffectAttackType effectType) {
	if (!pool.empty()) {
		// 從池中取出子彈並重置屬性
		auto effectAttack = pool.back();
		if (!effectAttack) LOG_ERROR("effectAttack = nullptr");
		effectAttack->ResetAll(type, bulletTransform, direction, size, damage, canReflect, effectType);
		pool.pop_back();  // 從池中移除
		effectAttack->SetActive(true);   // 激活子彈
		effectAttack->SetVisible(true);
		return effectAttack;
	}
	// 如果池是空的，創建一個新的子彈
	auto newEffectAttack = std::make_shared<EffectAttack>(type, bulletTransform, direction, size, damage, canReflect, effectType);
	return newEffectAttack;
}