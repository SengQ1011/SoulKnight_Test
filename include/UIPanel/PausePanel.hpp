//
// Created by QuzzS on 2025/3/4.
//

#ifndef PAUSEPANEL_HPP
#define PAUSEPANEL_HPP

#include <memory>
#include <vector>

#include "UIPanel.hpp"

class UIButton;
class nGameObject;
class TalentComponent;

class PausePanel : public UIPanel
{
public:
	explicit PausePanel(const std::shared_ptr<TalentComponent> &talentComp) : m_PlayerTalentComponent(talentComp) {}
	~PausePanel() override = default;
	void Start() override;
	void Update() override;
	void DrawDebugUI();

protected:
	// 天賦組件引用
	std::weak_ptr<TalentComponent> m_PlayerTalentComponent;

	// UI 元素
	std::shared_ptr<nGameObject> m_OverLay; // 遮罩層
	std::shared_ptr<nGameObject> m_PanelBackground;
	std::shared_ptr<UIButton> m_ResumeButton;
	std::shared_ptr<UIButton> m_MenuButton;
	std::shared_ptr<UIButton> m_SettingButton;

	// 天賦顯示
	std::vector<std::shared_ptr<nGameObject>> m_TalentIcons;
	static const int TALENT_SLOTS = 8; // 天賦槽位數量

private:
	void InitializeTalentIcons();
	void UpdateTalentIcons();
};

#endif // PAUSEPANEL_HPP
