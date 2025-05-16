//
// Created by tjx20 on 4/18/2025.
//

#ifndef EFFECTATTACK_HPP
#define EFFECTATTACK_HPP

#include "Attack/Attack.hpp"
class Animation;

namespace EffectAssets {
	inline std::string ResourcePath(const std::string& relative) {
		return std::string(RESOURCE_DIR) + relative;
	}

	inline const std::unordered_map<EffectAttackType, std::vector<std::string>> EFFECT_IMAGE_PATHS = {
		{EffectAttackType::SLASH, {
			ResourcePath("/Sprite/attackUI/slash/slash_0.png"),
			ResourcePath("/Sprite/attackUI/slash/slash_1.png"),
			ResourcePath("/Sprite/attackUI/slash/slash_2.png"),
			ResourcePath("/Sprite/attackUI/slash/slash_3.png")
		}},
		{EffectAttackType::LUNGE, {
			ResourcePath("/Sprite/attackUI/lunge/lunge_0.png"),
			ResourcePath("/Sprite/attackUI/lunge/lunge_1.png"),
			ResourcePath("/Sprite/attackUI/lunge/lunge_2.png"),
			ResourcePath("/Sprite/attackUI/lunge/lunge_3.png")
		}},
		{EffectAttackType::SHOCKWAVE,{
			ResourcePath("/Sprite/attackUI/shockwave/shockwave_0.png"),
			ResourcePath("/Sprite/attackUI/shockwave/shockwave_1.png"),
			ResourcePath("/Sprite/attackUI/shockwave/shockwave_2.png"),
			ResourcePath("/Sprite/attackUI/shockwave/shockwave_3.png"),
			ResourcePath("/Sprite/attackUI/shockwave/shockwave_4.png"),
			ResourcePath("/Sprite/attackUI/shockwave/shockwave_5.png"),
			ResourcePath("/Sprite/attackUI/shockwave/shockwave_6.png"),
			ResourcePath("/Sprite/attackUI/shockwave/shockwave_7.png")
		}}
	};
}

class EffectAttack : public Attack {
public:
	// 因爲斬擊/刺擊/震蕩波都是使用統一的動畫
	// 不需要在建構子中加入animation
	explicit EffectAttack(const CharacterType type, const Util::Transform &attackTransform, glm::vec2 direction, float size, int damage, bool canReflect, EffectAttackType effectType);

	//================ 提醒 ==============//
	void Init() override;
	void UpdateObject(float deltaTime) override;

	std::string GetName() const override { return "EffectAttack"; }

	//----Getter----
	[[nodiscard]] EffectAttackType GetEffectAttackType() const { return m_effectType; }
	[[nodiscard]] std::shared_ptr<Animation> GetAnimation()const { return m_animation; }
	[[nodiscard]] bool checkCanReflect() const { return m_reflectBullet; }

	//----Setter----
	void ResetAll(const CharacterType type, const Util::Transform &attackTransform, glm::vec2 direction, float size, int damage, bool canReflect,EffectAttackType effectType);

protected:
	EffectAttackType m_effectType;
	std::shared_ptr<Animation> m_animation;
	bool m_reflectBullet = false;
};

#endif //EFFECTATTACK_HPP
