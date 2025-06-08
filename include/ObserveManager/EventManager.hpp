//
// Created by QuzzS on 2025/4/29.
//

#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <functional>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>


#include "Scene/SceneManager.hpp"
#include "Structs/EventInfo.hpp"

class EventManager
{
public:
	using Listener = std::function<void(const EventInfo &)>;
	using ListenerID = size_t;

	static EventManager &GetInstance()
	{
		static EventManager instance;
		return instance;
	}

	// 基本訂閱方法（返回監聽器ID）
	ListenerID Subscribe(const std::type_index eventType, const Listener &listener)
	{
		ListenerID id = m_NextListenerID++;
		m_Listeners[eventType].emplace_back(id, listener);
		return id;
	}

	// 範本化訂閱方法，更類型安全（返回監聽器ID）
	template <typename EventT>
	ListenerID Subscribe(std::function<void(const EventT &)> listener)
	{
		auto wrapper = [listener](const EventInfo &info)
		{
			if (auto *specificEvent = dynamic_cast<const EventT *>(&info))
			{
				listener(*specificEvent);
			}
		};
		ListenerID id = m_NextListenerID++;
		m_Listeners[typeid(EventT)].emplace_back(id, wrapper);
		return id;
	}

	// 基於監聽器ID的取消訂閱
	template <typename EventT>
	bool Unsubscribe(ListenerID listenerID)
	{
		auto it = m_Listeners.find(typeid(EventT));
		if (it != m_Listeners.end())
		{
			auto &listeners = it->second;
			auto listenerIt = std::find_if(listeners.begin(), listeners.end(),
										   [listenerID](const std::pair<ListenerID, Listener> &pair)
										   { return pair.first == listenerID; });

			if (listenerIt != listeners.end())
			{
				listeners.erase(listenerIt);
				return true;
			}
		}
		return false;
	}

	// 發送事件
	void Notify(const EventInfo &info) const
	{
		auto it = m_Listeners.find(info.GetType());
		if (it != m_Listeners.end())
		{
			for (const auto &listenerPair : it->second)
			{
				listenerPair.second(info);
			}
		}
	}

	// 便利方法：直接發送特定事件
	template <typename EventT>
	void Emit(const EventT &event)
	{
		Notify(event);
	}

	// 取消訂閱（簡化版，清除所有該類型的監聽器）
	template <typename EventT>
	void UnsubscribeAll()
	{
		m_Listeners.erase(typeid(EventT));
	}

	// 清除所有監聽器
	void ClearAllListeners() { m_Listeners.clear(); }

	// ===== 便利的事件發送方法 =====

	// Camera抖動事件
	static void TriggerCameraShake(float duration = 0.3f, float intensity = 10.0f)
	{
		CameraShakeEvent event(duration, intensity);
		GetInstance().Notify(event);
	}

	// 保留原有的敵人死亡事件（向下兼容）
	static void enemyDeathEvent() { SceneManager::GetInstance().receiveEnemyDeathEvent(); }

private:
	std::unordered_map<std::type_index, std::vector<std::pair<ListenerID, Listener>>> m_Listeners;
	ListenerID m_NextListenerID = 1;
};

#endif // EVENTMANAGER_HPP
