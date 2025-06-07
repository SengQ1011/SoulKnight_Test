//
// Created by Assistant on 2025/1/20.
//

#include "Components/FlickerComponent.hpp"
#include "Override/nGameObject.hpp"
#include "Structs/EventInfo.hpp"

FlickerComponent::FlickerComponent() : Component(ComponentType::FLICKER) {}

void FlickerComponent::Update()
{
	if (!m_IsFlickering)
		return;

	m_FlickerTimer.Update();
}

void FlickerComponent::HandleEvent(const EventInfo &eventInfo)
{
	if (eventInfo.GetEventType() == EventType::StartFlicker)
	{
		const auto &flickerEvent = dynamic_cast<const StartFlickerEvent &>(eventInfo);
		StartFlicker(flickerEvent.duration, flickerEvent.interval);
	}
	else if (eventInfo.GetEventType() == EventType::Death)
	{
		// 角色死亡時立即停止閃爍並恢復可見性
		if (m_IsFlickering)
		{
			StopFlicker();
		}
	}
}

std::vector<EventType> FlickerComponent::SubscribedEventTypes() const
{
	return {
		EventType::StartFlicker,
		EventType::Death,
	};
}

void FlickerComponent::StartFlicker(float duration, float interval)
{
	auto owner = GetOwner<nGameObject>();
	if (!owner)
		return;

	m_IsFlickering = true;
	m_FlickerInterval = interval;
	m_OriginalVisibility = owner->IsControlVisible();

	// 設置總時長
	m_FlickerTimer.SetAndStart(duration);

	// 使用OnUpdate來控制閃爍
	m_FlickerTimer.SetOnUpdate(
		[this](float progress)
		{
			auto owner = GetOwner<nGameObject>();
			if (!owner)
				return;

			// 計算當前應該顯示還是隱藏
			float elapsedTime = m_FlickerTimer.GetElapsedTime();
			int flickerCycle = static_cast<int>(elapsedTime / m_FlickerInterval);
			bool shouldBeVisible = (flickerCycle % 2) == 0;

			// 根據原始可見性決定最終狀態
			owner->SetControlVisible(m_OriginalVisibility ? shouldBeVisible : !shouldBeVisible);
		});

	// 閃爍結束時的回調
	m_FlickerTimer.SetOnFinished([this]() { StopFlicker(); });
}

void FlickerComponent::StopFlicker()
{
	auto owner = GetOwner<nGameObject>();
	if (!owner)
		return;

	m_IsFlickering = false;
	m_FlickerTimer.Stop();
	m_FlickerTimer.ClearCallbacks();

	// 恢復可見
	owner->SetControlVisible(true);
}
