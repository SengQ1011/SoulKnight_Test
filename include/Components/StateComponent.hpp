//
// Created by tjx20 on 3/24/2025.
//

#ifndef STATECOMPONENT_HPP
#define STATECOMPONENT_HPP

#include "Components/Component.hpp"

namespace Core
{
	class Drawable;
}

struct StatusEffectVisual {
	bool isFullBodyEffect = false;	// 是否是全身覆蓋特效（如冰凍）
	std::string iconPath;			// 小 icon 的圖示路徑
	std::string fullBodyEffectPath; // 若是覆蓋角色的特效圖
};

static std::unordered_map<StatusEffect, StatusEffectVisual> effectVisualMap = {
	{ StatusEffect::BURNS,    { false, RESOURCE_DIR"/UI/icon/icon_burns.png", "" }},
	{ StatusEffect::POISON,  {  false, RESOURCE_DIR"/UI/icon/icon_poison.png","" }},
	{ StatusEffect::FROZEN,  {  true, "",RESOURCE_DIR"/UI/icon/icon_frozen.png" }},
};

class StateComponent final : public Component
{
public:
	explicit StateComponent();
	~StateComponent() override = default;

	void Init() override;
	void Update() override;

	/* ---Getter--- */
	State GetCurrentState() const { return m_currentState; }
	std::vector<StatusEffect> GetActiveEffects() { return m_statusEffects; }

	/* ---Setter--- */
	void SetState(State newState);
	void ApplyStatusEffect(const StatusEffect &effect); // 添加狀態異常
	void RemoveStatusEffect(const StatusEffect& effect);

	void HideAllIcon() const;
	void UpdateIconPosition();
	void ProcessStatusEffectTick(StatusEffect effect, float deltaTime);

private:
	State m_currentState;
	std::vector<StatusEffect> m_statusEffects;
	std::unordered_map<StatusEffect, float> m_statusEffectTimers;
	std::unordered_map<StatusEffect, std::shared_ptr<Core::Drawable>> m_allIcons;
	std::shared_ptr<nGameObject> m_icon1;
	std::shared_ptr<nGameObject> m_icon2;
};


#endif //STATECOMPONENT_HPP
