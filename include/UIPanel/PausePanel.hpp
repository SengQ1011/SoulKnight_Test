//
// Created by QuzzS on 2025/3/4.
//

#ifndef PAUSEPANEL_HPP
#define PAUSEPANEL_HPP

#include <memory>
#include <vector>

#include "UIPanel.hpp"
#include "glm/glm.hpp"

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

	// Override Show/Hide for animation
	void Show() override;
	void Hide() override;

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

	// 元件相對於面板背景的偏移量
	glm::vec2 m_BackgroundOffset = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_ResumeButtonOffset = glm::vec2(0.0f, -100.0f);
	glm::vec2 m_MenuButtonOffset = glm::vec2(-224.0f, -100.0f);
	glm::vec2 m_SettingButtonOffset = glm::vec2(224.0f, -100.0f);
	glm::vec2 m_TalentIconsBaseOffset = glm::vec2(0.0f, 0.0f); // 天賦圖標基準偏移

	// 動畫相關
	bool m_IsAnimating = false;
	bool m_IsShowingAnimation = false; // true = showing, false = hiding
	float m_AnimationTimer = 0.0f;
	float m_AnimationDuration = 0.4f; // 0.4秒動畫時間
	glm::vec2 m_VisiblePosition = glm::vec2(0.0f, 0.0f); // 顯示位置
	glm::vec2 m_HiddenPosition; // 隱藏位置，在 Start() 中計算

private:
	void InitializeTalentIcons();
	void UpdateTalentIcons();
	void StartShowAnimation();
	void StartHideAnimation();
	void UpdateAnimation();
	void UpdatePanelPosition(float progress);
	void UpdateAllElementsPosition(const glm::vec2 &panelPosition); // 統一更新所有元件位置
	float EaseOutQuad(float t); // 緩動函數
};

#endif // PAUSEPANEL_HPP
