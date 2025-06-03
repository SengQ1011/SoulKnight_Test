//
// Created by QuzzS on 2025/5/1.
//

#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


class UIPanel;

// 面板信息結構
struct PanelInfo
{
	std::shared_ptr<UIPanel> panel;
	int priority; // 優先級，數字越大優先級越高
	bool isModal; // 是否為模態面板（阻擋其他面板輸入）

	PanelInfo(std::shared_ptr<UIPanel> p, int prio, bool modal) : panel(std::move(p)), priority(prio), isModal(modal) {}
};

// 管理UI的顯示隱藏
class UIManager
{
public:
	static UIManager &GetInstance();

	void RegisterPanel(const std::string &name, const std::shared_ptr<UIPanel> &panel, int priority = 0,
					   bool isModal = false);
	void ResetPanels() { m_Panels.clear(); }

	void ShowPanel(const std::string &name);
	void HidePanel(const std::string &name);
	void TogglePanel(const std::string &name);

	bool IsPanelVisible(const std::string &name) const;
	bool ShouldBlockInput(const std::string &panelName) const; // 檢查是否應該阻擋輸入

	void Update();

private:
	std::unordered_map<std::string, PanelInfo> m_Panels;

	// 獲取當前最高優先級的可見模態面板
	std::string GetHighestModalPanel() const;

	UIManager() = default;
};

#endif // UIMANAGER_HPP
