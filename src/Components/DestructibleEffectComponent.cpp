//
// Created by Assistant on 2025/1/20.
//

#include "Components/DestructibleEffectComponent.hpp"

#include "Override/nGameObject.hpp"
#include "Structs/EventInfo.hpp"
#include "Util/Logger.hpp"

DestructibleEffectComponent::DestructibleEffectComponent(std::unique_ptr<IDestructionEffect> effect) :
	Component(ComponentType::DESTRUCTIBLE_EFFECT), m_effect(std::move(effect))
{
}

void DestructibleEffectComponent::Init()
{
	if (!m_effect)
	{
		LOG_ERROR("DestructibleEffectComponent initialized with null effect");
	}
}

void DestructibleEffectComponent::HandleEvent(const EventInfo &eventInfo)
{
	if (m_hasTriggered)
		return;

	// 檢查是否為箱子破壞事件
	if (eventInfo.GetEventType() == EventType::BoxBreak)
	{
		const auto owner = GetOwner<nGameObject>();
		if (!owner)
		{
			LOG_ERROR("DestructibleEffectComponent has no owner");
			return;
		}

		// 觸發破壞效果
		TriggerEffect(owner->GetWorldCoord(), CharacterType::PLAYER);
		m_hasTriggered = true;

		LOG_DEBUG("DestructibleEffectComponent triggered effect: {}", GetEffectName());
	}
}

std::vector<EventType> DestructibleEffectComponent::SubscribedEventTypes() const { return {EventType::BoxBreak}; }

void DestructibleEffectComponent::TriggerEffect(const glm::vec2 &position, CharacterType attackerType)
{
	if (!m_effect)
	{
		LOG_ERROR("Cannot trigger effect - effect is null");
		return;
	}

	if (m_hasTriggered)
	{
		LOG_WARN("Effect already triggered, ignoring duplicate trigger");
		return;
	}

	try
	{
		m_effect->Execute(position, attackerType);
		m_hasTriggered = true;
	}
	catch (const std::exception &e)
	{
		LOG_ERROR("Exception while triggering destruction effect: {}", e.what());
	}
}

std::string DestructibleEffectComponent::GetEffectName() const
{
	if (!m_effect)
	{
		return "NullEffect";
	}
	return m_effect->GetEffectName();
}
