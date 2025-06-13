//
// Created by Assistant on 2025/1/20.
//

#ifndef POISONCLOUDEFFECT_HPP
#define POISONCLOUDEFFECT_HPP

#include "IDestructionEffect.hpp"

/**
 * @brief 毒圈破壞效果
 * 用於綠色箱子，破壞時產生 POISON_AREA 效果攻擊
 * 使用 16 幀毒霧動畫，持續 4 秒，造成毒傷害
 */
class PoisonCloudEffect : public IDestructionEffect
{
public:
	explicit PoisonCloudEffect(float cloudSize = 64.0f, int poisonDamage = 3, float duration = 5.0f);
	~PoisonCloudEffect() override = default;

	void Execute(const glm::vec2 &position, CharacterType attackerType) override;
	std::string GetEffectName() const override { return "PoisonCloud"; }

private:
	float m_cloudSize; // 毒圈大小
	int m_poisonDamage; // 毒傷害
	float m_duration; // 持續時間
};

#endif // POISONCLOUDEFFECT_HPP
