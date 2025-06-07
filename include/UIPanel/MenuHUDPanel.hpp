//
// Created by QuzzS on 2025/6/5.
//

#ifndef MENUHUDPANEL_HPP
#define MENUHUDPANEL_HPP

#include <glm/glm.hpp>
#include <memory>
#include "UIPanel.hpp"
#include "Util/Timer.hpp"

class UIButton;
class nGameObject;

class MenuHUDPanel : public UIPanel
{
public:
	MenuHUDPanel() = default;
	~MenuHUDPanel() override = default;

	void Start() override;
	void Update() override;

	// Override Show/Hide for animation
	void Show() override;
	void Hide() override;

	// 檢查滑鼠點擊是否在按鈕上
	bool IsMouseClickingOnButtons() const;

protected:
	// 按鈕元件
	std::shared_ptr<UIButton> m_NewGameButton;
	std::shared_ptr<UIButton> m_ContinueGameButton;
	std::shared_ptr<nGameObject> m_NewGameButtonText;
	std::shared_ptr<nGameObject> m_ContinueGameButtonText;

	// 動畫相關
	bool m_IsAnimating = false;
	bool m_IsShowingAnimation = false; // true = showing, false = hiding
	Util::Timer m_AnimationTimer{0.5f}; // 動畫計時器，持續時間0.5秒

	// 位置設定
	glm::vec2 m_NewGameButtonVisiblePos = glm::vec2(-200.0f, -280.0f); // 左邊位置
	glm::vec2 m_ContinueGameButtonVisiblePos = glm::vec2(200.0f, -280.0f); // 右邊位置
	glm::vec2 m_NewGameButtonHiddenPos; // 隱藏位置，在Start()中計算
	glm::vec2 m_ContinueGameButtonHiddenPos; // 隱藏位置，在Start()中計算
	glm::vec2 m_TextOffset = glm::vec2(15.0f, 0.0f); // 文字相對於按鈕的偏移

	// 按鈕策略狀態
	bool m_HasGameProgress = false; // 是否有遊戲進度，決定顯示策略

private:
	void InitNewGameButton();
	void InitContinueGameButton();
	void StartShowAnimation();
	void StartHideAnimation();
	void UpdateAnimation();
	void UpdateButtonsPosition(float progress); // 緩動函數
};

#endif // MENUHUDPANEL_HPP
