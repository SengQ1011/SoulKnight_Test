//
// Created by QuzzS on 2025/5/1.
//

#include "UIPanel/UIManager.hpp"
#include <utility>
#include "UIPanel/UIPanel.hpp"

#include "Util/Logger.hpp"

UIManager &UIManager::GetInstance()
{
	static UIManager instance;
	return instance;
}

void UIManager::RegisterPanel(const std::string &name, const std::shared_ptr<UIPanel> &panel)
{
	m_Panels[name] = panel;
}

void UIManager::ShowPanel(const std::string &name)
{
	auto it = m_Panels.find(name);
	if (it != m_Panels.end())
	{
		it->second->Show();
	}
}

void UIManager::HidePanel(const std::string &name)
{
	auto it = m_Panels.find(name);
	if (it != m_Panels.end())
	{
		it->second->Hide();
	}
}

void UIManager::TogglePanel(const std::string &name)
{
	auto it = m_Panels.find(name);
	if (it != m_Panels.end())
	{
		auto &panel = it->second;
		if (panel->IsVisible())
			panel->Hide();
		else
			panel->Show();
	}
}

bool UIManager::IsPanelVisible(const std::string &name) const
{
	auto it = m_Panels.find(name);
	if (it != m_Panels.end())
	{
		return it->second->IsVisible();
	}
	return false;
}

void UIManager::Update()
{
	for (auto &[name, panel] : m_Panels)
	{
		if (panel->IsVisible())
		{
			panel->Update();
		}
	}
}
