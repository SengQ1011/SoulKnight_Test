//
// Created by QuzzS on 2025/5/1.
//

#ifndef SETTINGPANEL_HPP
#define SETTINGPANEL_HPP

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

protected:
	std::shared_ptr<nGameObject> m_OverLay; // 遮罩層
	std::shared_ptr<nGameObject> m_PanelBackground;
	std::shared_ptr<UISlider> m_SliderMasterVolume;
	std::shared_ptr<UISlider> m_SliderBGMVolume;
	std::shared_ptr<UISlider> m_SliderSFXVolume;
	std::shared_ptr<UIButton> m_CloseButton;
};

#endif // SETTINGPANEL_HPP
