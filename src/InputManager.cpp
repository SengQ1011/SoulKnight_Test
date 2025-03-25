//
// Created by tjx20 on 3/25/2025.
//

#include "InputManager.hpp"
#include "Util/Logger.hpp"

void InputManager::addObserver(const std::shared_ptr<Observer> &observer) {
	if (observer) {
		m_Observer.push_back(observer);
		LOG_DEBUG("Add New Observer");
	}else
	{
		LOG_ERROR("Failed to Add New Observer");
	}
}

void InputManager::removeObserver(const std::shared_ptr<Observer> &observer) {
	// vector/list/array的從begin到end的搜索法：std::find(vec.begin(), vec.end, XXX)
	auto it = std::find(m_Observer.begin(), m_Observer.end(), observer);
	if (it != m_Observer.end()) {
		m_Observer.erase(it);
	}
}

void InputManager::onKeyPressed(char key) {
	m_activeKeys.insert(key);
	notifyObserver();
}

void InputManager::onKeyReleased(char key) {
	m_activeKeys.erase(key);
	notifyObserver();
}

void InputManager::notifyObserver() {
	for (const auto& observer : m_Observer) {
		observer->onInputReceived(m_activeKeys);
	}
}