//
// Created by Assistant on 2025/1/20.
//

#include "Components/DropComponent.hpp"
#include <glm/gtc/constants.hpp>
#include "Override/nGameObject.hpp"
#include "RandomUtil.hpp"
#include "Scene/SceneManager.hpp"
#include "Structs/EventInfo.hpp"
#include "Util/Logger.hpp"

DropComponent::DropComponent(ScatterMode mode, float radius) :
	Component(ComponentType::DROP), m_scatterMode(mode), m_scatterRadius(radius)
{
}

void DropComponent::Init()
{
	// 初始化時不需要特別處理
}

void DropComponent::HandleEvent(const EventInfo &eventInfo)
{
	if (m_hasDropped)
		return;

	// 檢查是否為箱子破壞事件或寶箱開啟事件
	if (eventInfo.GetEventType() == EventType::BoxBreak || eventInfo.GetEventType() == EventType::ChestOpen)
	{
		const auto owner = GetOwner<nGameObject>();
		if (!owner)
		{
			LOG_ERROR("DropComponent has no owner");
			return;
		}

		// 觸發掉落效果
		TriggerDrop(owner->GetWorldCoord());
		m_hasDropped = true;

		LOG_DEBUG("DropComponent triggered drop at position ({}, {})", owner->GetWorldCoord().x,
				  owner->GetWorldCoord().y);
	}
}

std::vector<EventType> DropComponent::SubscribedEventTypes() const
{
	return {EventType::BoxBreak, EventType::ChestOpen};
}

void DropComponent::TriggerDrop(const glm::vec2 &position)
{
	if (m_hasDropped)
		return;
	m_hasDropped = true;
	const auto interactionManager =
		SceneManager::GetInstance().GetCurrentScene().lock()->GetCurrentRoom()->GetInteractionManager();

	for (const auto &dropItem : m_dropItems)
	{
		if (!dropItem)
			continue;

		glm::vec2 dropPosition = position;

		// 根據散佈模式計算位置
		switch (m_scatterMode)
		{
		case ScatterMode::FIXED:
			// 固定在中心位置，不偏移
			break;
		case ScatterMode::SLIGHT:
			{
				// 輕微散佈
				float angle = RandomUtil::RandomFloatInRange(0.0f, 360.0f);
				float distance = RandomUtil::RandomFloatInRange(5.0f, 15.0f);
				glm::vec2 offset = glm::vec2(cos(glm::radians(angle)) * distance, sin(glm::radians(angle)) * distance);
				dropPosition += offset;
				break;
			}
		case ScatterMode::RANDOM:
		default:
			{
				// 隨機散佈位置
				float angle = RandomUtil::RandomFloatInRange(0.0f, 360.0f);
				float distance = RandomUtil::RandomFloatInRange(10.0f, m_scatterRadius);
				glm::vec2 offset = glm::vec2(cos(glm::radians(angle)) * distance, sin(glm::radians(angle)) * distance);
				dropPosition += offset;
				break;
			}
		}

		// 設置物件位置並激活
		dropItem->SetWorldCoord(dropPosition);
		dropItem->SetActive(true);
		dropItem->SetControlVisible(true);

		interactionManager->RegisterInteractable(dropItem);

		m_dropItems.clear();

		LOG_DEBUG("Dropped item at position ({}, {})", dropPosition.x, dropPosition.y);
	}
}

void DropComponent::AddDropItems(const std::vector<std::shared_ptr<nGameObject>> &dropItems)
{
	m_dropItems = dropItems;
}
