//
// Created by Assistant on 2025/1/20.
//

#ifndef ICESPIKEEFFECT_HPP
#define ICESPIKEEFFECT_HPP

#include "IDestructionEffect.hpp"

/**
 * @brief 冰刺破壞效果
 * 用於藍色箱子，破壞時向8個方向發射冰刺
 */
class IceSpikeEffect : public IDestructionEffect
{
public:
	explicit IceSpikeEffect(int spikeCount = 8, int spikeDamage = 4, float spikeSpeed = 200.0f);
	~IceSpikeEffect() override = default;

	void Execute(const glm::vec2 &position, CharacterType attackerType) override;
	std::string GetEffectName() const override { return "IceSpike"; }

private:
	int m_spikeCount; // 冰刺數量
	int m_spikeDamage; // 冰刺傷害
	float m_spikeSpeed; // 冰刺速度
};

#endif // ICESPIKEEFFECT_HPP
