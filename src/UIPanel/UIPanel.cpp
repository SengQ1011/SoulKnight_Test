//
// Created by QuzzS on 2025/5/1.
//

#include "UIPanel/UIPanel.hpp"
#include "Override/nGameObject.hpp"

void UIPanel::Show() {
	m_IsVisible = true;
	for (const auto& element : m_GameObjects) {
		element->SetControlVisible(true);
		element->SetActive(true); // 接受事件
	}
}

void UIPanel::Hide() {
	m_IsVisible = false;
	for (const auto & element : m_GameObjects) {
		element->SetControlVisible(false);
		element->SetActive(false);
	}
}
