//
// Created by Assistant on 2025/1/20.
//

#ifndef DESTRUCTIBLEEFFECTCOMPONENT_HPP
#define DESTRUCTIBLEEFFECTCOMPONENT_HPP

#include <memory>
#include "Components/Component.hpp"
#include "DestructionEffects/IDestructionEffect.hpp"


/**
 * @brief 破壞效果組件
 * 負責監聽物體破壞事件並觸發相應的破壞效果
 */
class DestructibleEffectComponent : public Component
{
public:
	explicit DestructibleEffectComponent(std::unique_ptr<IDestructionEffect> effect);
	~DestructibleEffectComponent() override = default;

	void Init() override;
	void HandleEvent(const EventInfo &eventInfo) override;
	std::vector<EventType> SubscribedEventTypes() const override;

	/**
	 * @brief 觸發破壞效果
	 * @param position 效果觸發位置
	 * @param attackerType 攻擊者類型
	 */
	void TriggerEffect(const glm::vec2 &position, CharacterType attackerType);

	/**
	 * @brief 獲取效果名稱
	 * @return 效果名稱
	 */
	std::string GetEffectName() const;

private:
	std::unique_ptr<IDestructionEffect> m_effect; // 破壞效果策略
	bool m_hasTriggered = false; // 是否已觸發，防止重複觸發
};

#endif // DESTRUCTIBLEEFFECTCOMPONENT_HPP
 