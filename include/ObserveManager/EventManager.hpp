//
// Created by QuzzS on 2025/4/29.
//

#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <functional>
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

	static EventManager &GetInstance()
	{
		static EventManager instance;
		return instance;
	}

	// 基本訂閱方法
	void Subscribe(const std::type_index eventType, const Listener &listener)
	{
		m_Listeners[eventType].push_back(listener);
	}

	// 範本化訂閱方法，更類型安全
	template <typename EventT>
	void Subscribe(std::function<void(const EventT &)> listener)
	{
		auto wrapper = [listener](const EventInfo &info)
		{
			if (auto *specificEvent = dynamic_cast<const EventT *>(&info))
			{
				listener(*specificEvent);
			}
		};
		m_Listeners[typeid(EventT)].push_back(wrapper);
	}

	// 發送事件
	void Notify(const EventInfo &info) const
	{
		auto it = m_Listeners.find(info.GetType());
		if (it != m_Listeners.end())
		{
			for (const auto &listener : it->second)
			{
				listener(info);
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
	void Unsubscribe()
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
	std::unordered_map<std::type_index, std::vector<Listener>> m_Listeners;
};

#endif // EVENTMANAGER_HPP
