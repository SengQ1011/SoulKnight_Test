//
// Created by QuzzS on 2025/4/29.
//

#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>
#include "Structs/EventInfo.hpp"

class EventManager {
public:
	using Listener = std::function<void(const EventInfo&)>;

	static EventManager& GetInstance() {
		static EventManager instance;
		return instance;
	}

	void Subscribe(const std::type_index eventType, const Listener& listener) {
		m_Listeners[eventType].push_back(listener);
	}

	void Notify(const EventInfo& info) const {
		auto it = m_Listeners.find(info.GetType());
		if (it != m_Listeners.end()) {
			for (const auto& listener : it->second) {
				listener(info);
			}
		}
	}

private:
	std::unordered_map<std::type_index, std::vector<Listener>> m_Listeners;
};

#endif //EVENTMANAGER_HPP
