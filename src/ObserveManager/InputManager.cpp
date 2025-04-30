//
// Created by tjx20 on 3/25/2025.
//

#include "ObserveManager/InputManager.hpp"

#include "Observer.hpp"
#include "Util/Input.hpp"

void InputManager::Update() {
	static const std::array<std::pair<Util::Keycode, char>, 12> keys = {{
		{Util::Keycode::W, 'W'},	// 上
		{Util::Keycode::S, 'S'},	// 下
		{Util::Keycode::A, 'A'},	// 左
		{Util::Keycode::D, 'D'},	// 右
		{Util::Keycode::E, 'E'},	// 開啓/打開
		{Util::Keycode::J, 'J'},	// 攻擊
		{Util::Keycode::L, 'L'},	// 切換武器
		{Util::Keycode::U, 'U'},	// 技能
		{Util::Keycode::I, 'I'},
		{Util::Keycode::K, 'K'},
		{Util::Keycode::P, 'P'},	// 暫停
		{Util::Keycode::M, 'M'}
	}};

	for (const auto& [keycode, key] : keys) {
		if (Util::Input::IsKeyPressed(keycode)) {
			onKeyPressed(key);
		} else {
			onKeyReleased(key);
		}
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