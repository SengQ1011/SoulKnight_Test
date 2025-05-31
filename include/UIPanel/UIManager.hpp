//
// Created by QuzzS on 2025/5/1.
//

#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include <string>
#include <memory>
#include <unordered_map>
class UIPanel;

//管理UI的顯示隱藏
class UIManager {
public:
	static UIManager& GetInstance();

	void RegisterPanel(const std::string& name, const std::shared_ptr<UIPanel>& panel);
	void ResetPanels() {m_Panels.clear();}

	void ShowPanel(const std::string& name);
	void HidePanel(const std::string& name);
	void TogglePanel(const std::string& name);

	void Update();

private:
	std::unordered_map<std::string, std::shared_ptr<UIPanel>> m_Panels;

	UIManager() = default;
};

#endif //UIMANAGER_HPP
