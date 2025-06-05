//
// Created by QuzzS on 2025/3/4.
//

#ifndef MAINMENU_SCENE_HPP
#define MAINMENU_SCENE_HPP

#include "Scene/Scene.hpp"

#include "Util/BGM.hpp"
#include "Util/SFX.hpp"


class UIButton;
class UIPanel;
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
	std::shared_ptr<Util::GameObject> m_Background = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_Title = std::make_shared<Util::GameObject>();
	std::shared_ptr<nGameObject> m_RedShawl;

	std::shared_ptr<Util::GameObject> m_Text = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_Version = std::make_shared<Util::GameObject>();
	std::shared_ptr<UIButton> m_SettingButton;
	std::shared_ptr<UIButton> m_DeleteDataButton;

	// 新遊戲和繼續遊戲按鈕
	std::shared_ptr<UIButton> m_NewGameButton;
	std::shared_ptr<UIButton> m_ContinueGameButton;
	std::shared_ptr<Util::GameObject> m_NewGameButtonText = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_ContinueGameButtonText = std::make_shared<Util::GameObject>();

	// 平移動畫相關變數
	bool m_IsSliding = false;
	float m_SlideTimer = 0.0f;
	float m_SlideDuration = 1.0f; // 1秒平移時間
	glm::vec2 m_StartPosition; // 起始位置（螢幕右邊）
	glm::vec2 m_TargetPosition; // 目標位置

	// 文字閃爍效果相關變數
	float m_TextBlinkTimer = 0.0f;
	float m_TextBlinkPeriod = 1.5f; // 1.5秒完成一個閃爍週期

	void InitBackground();
	void InitTitleAndDecor();
	void InitTextLabels();
	void InitSettingButton();
	void InitDeleteDataButton();
	void InitUIManager();
	void InitAudioManager();

	// 新遊戲和繼續遊戲按鈕初始化
	void InitNewGameButton();
	void InitContinueGameButton();

	// 動畫相關函式
	void UpdateSlideAnimation();
	void UpdateTextBlinkEffect();
	void StartSlideAnimation();

	// 檢查滑鼠是否點擊到按鈕上
	bool IsMouseClickingOnButtons() const;
};

#endif // MAINMENU_SCENE_HPP
