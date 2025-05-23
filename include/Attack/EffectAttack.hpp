//
// Created by tjx20 on 4/18/2025.
//

#ifndef EFFECTATTACK_HPP
#define EFFECTATTACK_HPP

#include "Attack/Attack.hpp"

class Animation;

struct  EffectAttackInfo : public AttackInfo
{
	~EffectAttackInfo() override = default;
	EffectAttackType effectType = EffectAttackType::NONE;
	bool canReflectBullet = false;
	bool canBlockingBullet = true;
};

namespace EffectAssets {
	inline std::string ResourcePath(const std::string& relative) {
		return std::string(RESOURCE_DIR) + relative;
	}

	inline const std::unordered_map<EffectAttackType, std::vector<std::string>> EFFECT_IMAGE_PATHS = {
		{EffectAttackType::SLASH, {
			ResourcePath("/attackUI/slash/slash_0.png"),
			ResourcePath("/attackUI/slash/slash_1.png"),
			ResourcePath("/attackUI/slash/slash_2.png"),
			ResourcePath("/attackUI/slash/slash_3.png")
		}},
		{EffectAttackType::LUNGE, {
			ResourcePath("/attackUI/lunge/lunge_0.png"),
			ResourcePath("/attackUI/lunge/lunge_1.png"),
			ResourcePath("/attackUI/lunge/lunge_2.png"),
			ResourcePath("/attackUI/lunge/lunge_3.png")
		}},
		{EffectAttackType::SHOCKWAVE,{
			ResourcePath("/attackUI/shockwave/shockwave_0.png"),
			ResourcePath("/attackUI/shockwave/shockwave_1.png"),
			ResourcePath("/attackUI/shockwave/shockwave_2.png"),
			ResourcePath("/attackUI/shockwave/shockwave_3.png"),
			ResourcePath("/attackUI/shockwave/shockwave_4.png"),
			ResourcePath("/attackUI/shockwave/shockwave_5.png"),
			ResourcePath("/attackUI/shockwave/shockwave_6.png"),
			ResourcePath("/attackUI/shockwave/shockwave_7.png")
		}},
		{EffectAttackType::ENERGY_WAVE,{
			ResourcePath("/attackUI/energy_wave/energy_wave_0.png"),
			ResourcePath("/attackUI/energy_wave/energy_wave_1.png"),
			ResourcePath("/attackUI/energy_wave/energy_wave_2.png"),
			ResourcePath("/attackUI/energy_wave/energy_wave_3.png"),
			ResourcePath("/attackUI/energy_wave/energy_wave_4.png")
		}},
		{EffectAttackType::LARGE_BOOM,{
			ResourcePath("/attackUI/boom/large_boom/element_exploded_fire_large_0.png"),
			ResourcePath("/attackUI/boom/large_boom/element_exploded_fire_large_1.png"),
			ResourcePath("/attackUI/boom/large_boom/element_exploded_fire_large_2.png"),
			ResourcePath("/attackUI/boom/large_boom/element_exploded_fire_large_3.png"),
			ResourcePath("/attackUI/boom/large_boom/element_exploded_fire_large_4.png")
		}},
		{EffectAttackType::MEDIUM_BOOM,{
			ResourcePath("/attackUI/boom/medium_boom/effect_explode_1.png"),
			ResourcePath("/attackUI/boom/medium_boom/effect_explode_2.png"),
			ResourcePath("/attackUI/boom/medium_boom/effect_explode_3.png"),
			ResourcePath("/attackUI/boom/medium_boom/effect_explode_4.png"),
			ResourcePath("/attackUI/boom/medium_boom/effect_explode_5.png"),
			ResourcePath("/attackUI/boom/medium_boom/effect_explode_6.png"),
			ResourcePath("/attackUI/boom/medium_boom/effect_explode_7.png"),
			ResourcePath("/attackUI/boom/medium_boom/effect_explode_8.png"),
			ResourcePath("/attackUI/boom/medium_boom/effect_explode_9.png")
		}},
	};
}

class EffectAttack : public Attack {
public:
	// 因爲斬擊/刺擊/震蕩波都是使用統一的動畫
	// 不需要在建構子中加入animation
	explicit EffectAttack(const EffectAttackInfo &effect_attack_info);
	~EffectAttack() override = default;

	//================ 提醒 ==============//
	void Init() override;
	void UpdateObject(float deltaTime) override;

	//----Getter----
	std::string GetName() const override { return "EffectAttack"; }
	[[nodiscard]] EffectAttackType GetEffectAttackType() const { return m_effectType; }
	[[nodiscard]] std::shared_ptr<Animation> GetAnimation()const { return m_animation; }
	[[nodiscard]] bool checkCanReflect() const { return m_reflectBullet; }

	//----Setter----
	void ResetAll(const EffectAttackInfo &effectAttackInfo);

protected:
	EffectAttackType m_effectType;
	std::shared_ptr<Animation> m_animation;
	bool m_reflectBullet = false;
	bool m_bulletBlocking = true;
	float m_shockwaveForce = 120.0f;
};

#endif //EFFECTATTACK_HPP
