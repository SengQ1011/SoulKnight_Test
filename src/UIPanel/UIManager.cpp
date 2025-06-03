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

void UIManager::RegisterPanel(const std::string &name, const std::shared_ptr<UIPanel> &panel, int priority,
							  bool isModal)
{
	m_Panels.emplace(name, PanelInfo(panel, priority, isModal));
}

void UIManager::ShowPanel(const std::string &name)
{
	auto it = m_Panels.find(name);
	if (it != m_Panels.end())
	{
		it->second.panel->Show();
	}
}

void UIManager::HidePanel(const std::string &name)
{
	auto it = m_Panels.find(name);
	if (it != m_Panels.end())
	{
		it->second.panel->Hide();
	}
}

void UIManager::TogglePanel(const std::string &name)
{
	auto it = m_Panels.find(name);
	if (it != m_Panels.end())
	{
		auto &panel = it->second.panel;
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
		return it->second.panel->IsVisible();
	}
	return false;
}

bool UIManager::ShouldBlockInput(const std::string &panelName) const
{
	auto it = m_Panels.find(panelName);
	if (it == m_Panels.end())
		return false;

	const int currentPanelPriority = it->second.priority;

	// 檢查是否有更高優先級的模態面板正在顯示
	for (const auto &[name, info] : m_Panels)
	{
		if (info.panel->IsVisible() && info.isModal && info.priority > currentPanelPriority)
		{
			return true; // 有更高優先級的模態面板，應該阻擋輸入
		}
	}

	return false; // 沒有更高優先級的模態面板，不阻擋輸入
}

std::string UIManager::GetHighestModalPanel() const
{
	std::string highestModalPanel;
	int highestPriority = -1;

	for (const auto &[name, info] : m_Panels)
	{
		if (info.panel->IsVisible() && info.isModal && info.priority > highestPriority)
		{
			highestPriority = info.priority;
			highestModalPanel = name;
		}
	}

	return highestModalPanel;
}

void UIManager::Update()
{
	// 獲取當前最高優先級的模態面板
	std::string highestModal = GetHighestModalPanel();

	for (auto &[name, info] : m_Panels)
	{
		if (info.panel->IsVisible())
		{
			// 如果有模態面板且當前面板不是最高優先級的模態面板，則跳過 Update
			if (!highestModal.empty() && name != highestModal && !info.isModal)
			{
				continue; // 跳過低優先級面板的 Update
			}
			// 如果當前面板是模態的但不是最高優先級的模態面板，也跳過
			else if (!highestModal.empty() && name != highestModal && info.isModal &&
					 info.priority < m_Panels.at(highestModal).priority)
			{
				continue; // 跳過低優先級模態面板的 Update
			}

			info.panel->Update();
		}
	}
}
