//
// Created by Assistant on 2025/1/20.
//

#ifndef EXPLOSIONEFFECT_HPP
#define EXPLOSIONEFFECT_HPP

#include "IDestructionEffect.hpp"

/**
 * @brief 爆炸破壞效果
 * 用於紅色箱子，破壞時產生爆炸特效和傷害
 */
class ExplosionEffect : public IDestructionEffect
{
public:
	explicit ExplosionEffect(float explosionSize = 36.0f, int explosionDamage = 6);
	~ExplosionEffect() override = default;

	void Execute(const glm::vec2 &position, CharacterType attackerType) override;
	std::string GetEffectName() const override { return "Explosion"; }

private:
	float m_explosionSize; // 爆炸範圍
	int m_explosionDamage; // 爆炸傷害
};

#endif // EXPLOSIONEFFECT_HPP
