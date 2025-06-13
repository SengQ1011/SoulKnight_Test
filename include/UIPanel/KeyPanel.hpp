//
// Created by QuzzS on 2025/5/1.
//

#ifndef KEYPANEL_HPP
#define KEYPANEL_HPP

#include <glm/glm.hpp>
#include <memory>

#include "UIPanel.hpp"
#include "Util/Timer.hpp"

class UIButton;
class nGameObject;

class KeyPanel : public UIPanel
{
public:
	KeyPanel() = default;
	~KeyPanel() override = default;
	void Start() override;
	void Update() override;
	void DrawDebugUI();

	// Override Show/Hide for animation
	void Show() override;
	void Hide() override;

protected:
	std::shared_ptr<nGameObject> m_OverLay; // 遮罩層
	std::shared_ptr<nGameObject> m_PanelBackground; // 按鍵說明背景
	std::shared_ptr<UIButton> m_CloseButton; // 關閉按鈕

	// 側邊按鈕
	std::shared_ptr<nGameObject> m_SideButton1; // 左上第一個 - nGameObject (純顯示)
	std::shared_ptr<UIButton> m_SideButton2; // 第二個 - UIButton (可點擊，切換到 SettingPanel)

	// 元件相對於面板背景的偏移量
	glm::vec2 m_BackgroundOffset = glm::vec2(0.0f, 22.0f);
	glm::vec2 m_CloseButtonOffset = glm::vec2(335.0f, 197.0f); // 右上角位置

	// 動畫相關
	bool m_IsAnimating = false;
	bool m_IsShowingAnimation = false; // true = showing, false = hiding
	Util::Timer m_AnimationTimer{0.4f}; // 動畫計時器，持續時間0.4秒
	glm::vec2 m_VisiblePosition = glm::vec2(22.0f); // 顯示位置
	glm::vec2 m_HiddenPosition; // 隱藏位置，在 Start() 中計算

private:
	void StartShowAnimation();
	void StartHideAnimation();
	void UpdateAnimation();
	void UpdatePanelPosition(float progress);
	void UpdateAllElementsPosition(const glm::vec2 &panelPosition); // 統一更新所有元件位置

	// 側邊按鈕初始化和位置計算
	void InitializeSideButtons();
	void UpdateSideButtonsPosition(const glm::vec2 &panelPosition);
};

#endif // KEYPANEL_HPP
