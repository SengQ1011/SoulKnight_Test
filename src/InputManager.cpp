//
// Created by tjx20 on 3/25/2025.
//

#include "InputManager.hpp"
#include "Util/Logger.hpp"

void InputManager::listenInput() {
	static const std::array<std::pair<Util::Keycode, char>, 9> keys = {{
		{Util::Keycode::W, 'W'},	// 上
		{Util::Keycode::S, 'S'},	// 下
		{Util::Keycode::A, 'A'},	// 左
		{Util::Keycode::D, 'D'},	// 右
		{Util::Keycode::E, 'E'},	// 開啓/打開
		{Util::Keycode::J, 'J'},	// 攻擊
		{Util::Keycode::U, 'U'},	// 切換武器
		{Util::Keycode::I, 'I'},
		{Util::Keycode::K, 'K'}
	}};

	for (const auto& [keycode, key] : keys) {
		if (Util::Input::IsKeyPressed(keycode)) {
			onKeyPressed(key);
		} else {
			onKeyReleased(key);
		}
	}
}



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