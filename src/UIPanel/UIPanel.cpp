//
// Created by QuzzS on 2025/5/1.
//

#include "UIPanel/UIPanel.hpp"
#include "Override/nGameObject.hpp"
#include "UIPanel/UIManager.hpp"


void UIPanel::Show()
{
	m_IsVisible = true;
	for (const auto &element : m_GameObjects)
	{
		element->SetVisible(true);
		element->SetActive(true); // 接受事件
	}
}

void UIPanel::Hide()
{
	m_IsVisible = false;
	for (const auto &element : m_GameObjects)
	{
		element->SetVisible(false);
		element->SetActive(false);
	}
}

bool UIPanel::ShouldBlockInput() const { return UIManager::GetInstance().ShouldBlockInput(m_PanelName); }
