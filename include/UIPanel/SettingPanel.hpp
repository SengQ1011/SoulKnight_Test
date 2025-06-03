//
// Created by QuzzS on 2025/5/1.
//

#ifndef SETTINGPANEL_HPP
#define SETTINGPANEL_HPP

#include <glm/glm.hpp>
#include <memory>


#include "UIPanel.hpp"

class UIButton;
class UISlider;
class nGameObject;

class SettingPanel : public UIPanel
{
public:
	SettingPanel() = default;
	~SettingPanel() override = default;
	void Start() override;
	void Update() override;
	void DrawDebugUI();

	// Override Show/Hide for animation
	void Show() override;
	void Hide() override;

protected:
	std::shared_ptr<nGameObject> m_OverLay; // 遮罩層
	std::shared_ptr<nGameObject> m_PanelBackground;
	std::shared_ptr<UISlider> m_SliderMasterVolume;
	std::shared_ptr<UISlider> m_SliderBGMVolume;
	std::shared_ptr<UISlider> m_SliderSFXVolume;
	std::shared_ptr<UIButton> m_CloseButton;

	// 元件相對於面板背景的偏移量
	glm::vec2 m_BackgroundOffset = glm::vec2(0.0f, 22.0f);
	glm::vec2 m_SliderOffset1 = glm::vec2(50.0f, 70.0f);
	glm::vec2 m_SliderOffset2 = glm::vec2(50.0f, -55.0f);
	glm::vec2 m_SliderOffset3 = glm::vec2(50.0f, -180.0f);
	glm::vec2 m_CloseButtonOffset = glm::vec2(335.0f, 197.0f);

	// 動畫相關
	bool m_IsAnimating = false;
	bool m_IsShowingAnimation = false; // true = showing, false = hiding
	float m_AnimationTimer = 0.0f;
	float m_AnimationDuration = 0.4f; // 0.4秒動畫時間
	glm::vec2 m_VisiblePosition = glm::vec2(22.0f); // 顯示位置
	glm::vec2 m_HiddenPosition; // 隱藏位置，在 Start() 中計算

private:
	void StartShowAnimation();
	void StartHideAnimation();
	void UpdateAnimation();
	void UpdatePanelPosition(float progress);
	void UpdateAllElementsPosition(const glm::vec2 &panelPosition); // 統一更新所有元件位置
	float EaseOutQuad(float t); // 緩動函數
};

#endif // SETTINGPANEL_HPP
