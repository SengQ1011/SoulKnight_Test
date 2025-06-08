//
// Created by Assistant on 2025/1/20.
//

#include "RoomObject/DestructibleBox.hpp"
#include "Components/HealthComponent.hpp"
#include "Structs/EventInfo.hpp"
#include "Util/Logger.hpp"

DestructibleBox::DestructibleBox(const std::string &baseName) : nGameObject(baseName, "DestructibleBox")
{
	// 不在建構函數中添加組件，而是在工廠中添加
}


void DestructibleBox::OnEventReceived(const EventInfo &eventInfo)
{
	// 處理破壞事件
	if (eventInfo.GetEventType() == EventType::BoxBreak)
	{
		LOG_INFO("EVENT RECEIVED BoxBreak");
		OnDestroyed();
	}
}

void DestructibleBox::OnDestroyed()
{
	if (m_IsDestroyed)
		return;

	m_IsDestroyed = true;

	LOG_DEBUG("DestructibleBox destroyed: {}", GetName());

	// 可以在這裡添加破壞效果：
	// - 播放破壞動畫
	// - 產生掉落物品
	// - 播放破壞音效
	// - 移除碰撞體

	// 暫時簡單地隱藏物件
	SetActive(false);
	SetControlVisible(false);
}
