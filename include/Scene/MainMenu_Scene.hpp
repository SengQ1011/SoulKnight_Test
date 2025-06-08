//
// Created by QuzzS on 2025/3/4.
//

#ifndef MAINMENU_SCENE_HPP
#define MAINMENU_SCENE_HPP

#include "Scene/Scene.hpp"

#include "Util/BGM.hpp"
#include "Util/SFX.hpp"
#include "Util/Timer.hpp"

class UIButton;
class UIPanel;
class MenuHUDPanel;
class MainMenuScene : public Scene
{
public:
	MainMenuScene() : Scene(SceneType::Menu) {}
	~MainMenuScene() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	void Upload() override; // 覆寫以避免上傳錯誤數據
	SceneType Change() override;

protected:
	std::shared_ptr<nGameObject> m_Background;
	std::shared_ptr<nGameObject> m_Title;
	std::shared_ptr<nGameObject> m_RedShawl;

	std::shared_ptr<nGameObject> m_Text;
	std::shared_ptr<nGameObject> m_Version;
	std::shared_ptr<UIButton> m_SettingButton;
	std::shared_ptr<UIButton> m_DeleteDataButton;

	// MenuHUDPanel 替代個別按鈕
	std::shared_ptr<MenuHUDPanel> m_MenuHUDPanel;

	// 動畫相關變數
	Util::Timer m_SlideTimer{0.4f}; // 平移動畫計時器，持續時間0.4秒

	glm::vec2 m_StartPosition; // 起始位置（螢幕右邊）
	glm::vec2 m_TargetPosition; // 目標位置

	// 文字閃爍效果相關
	float m_TextBlinkPeriod = 1.5f; // 閃爍週期（秒）
	float m_TextBlinkTime = 0.0f; // 文字閃爍累積時間

	void InitBackground();
	void InitTitleAndDecor();
	void InitTextLabels();
	void InitSettingButton();
	void InitDeleteDataButton();
	void InitUIManager();
	void InitAudioManager();

	// 初始化MenuHUDPanel
	void InitMenuHUDPanel();

	// 動畫相關函式
	void InitSlideAnimation();
	void UpdateAnimations();
	void UpdateTextBlinkEffect(); // 持續的文字閃爍效果
	void StartSlideAnimation();

	// 檢查滑鼠是否點擊到按鈕上
	bool IsMouseClickingOnButtons() const;

	// 切換面板狀態變數
	bool m_ShowMenuButtons = false;
};

#endif // MAINMENU_SCENE_HPP
