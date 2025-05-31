//
// Created by QuzzS on 2025/3/15.
//

// nGameObject.inl

#ifndef NGAMEOBJECT_INL
#define NGAMEOBJECT_INL

#include "nGameObject.hpp"
#include "Components/AllComponentInclude.hpp"

template <typename T, typename... Args>
std::shared_ptr<T> nGameObject::AddComponent(ComponentType type,Args &&...args) //或許weak_ptr
{
	//確保T是Component子類別的安全判斷，如果不用樣板直接用Component，那每次使用Function就要手動ObjectCast了
	static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
	auto component = std::make_shared<T>(std::forward<Args>(args)...);
	component->SetOwner(shared_from_this());
	m_Components[type] = component;
	component->Init();
	// 讓Component註冊感興趣的事件型別
	for (const auto& eventType : component->SubscribedEventTypes())
	{
		m_EventSubscribers[eventType].push_back(component.get());
	}
	return component;
}

template <typename T>
std::shared_ptr<T> nGameObject::GetComponent(const ComponentType type) //或許weak_ptr
{
	// unordermap搜索法：unordered_map.find()
	auto it = m_Components.find(type);
	if (it != m_Components.end()) {
		auto casted = std::dynamic_pointer_cast<T>(it->second);
		if (casted) return casted;
		else LOG_WARN("GetComponent failed: type mismatch for {}", static_cast<int>(type));
	}
	return nullptr;
}

template <typename EventT>
void nGameObject::OnEvent(const EventT &eventInfo)
{
	if (!m_Active)
		return;

	// 留條後路，讓nGameObject自己處理Event的邏輯 - Projectile害的
	OnEventReceived(eventInfo);

	const EventType eventType = eventInfo.GetEventType();
	if (const auto it = m_EventSubscribers.find(eventType); it != m_EventSubscribers.end())
	{
		for (auto* component : it->second)
		{
			component->HandleEvent(eventInfo);
		}
	}
}



#endif //NGAMEOBJECT_INL
