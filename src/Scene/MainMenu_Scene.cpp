//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/MainMenu_Scene.hpp"

#include <iostream>

#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Tool/Tool.hpp"
#include "UIPanel/SettingPanel.hpp"
#include "UIPanel/UIButton.hpp"
#include "UIPanel/UIManager.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Text.hpp"
#include "config.hpp"

void MainMenuScene::Start()
{
	LOG_DEBUG("Entering Main Menu");

	InitBackground();
	InitTitleAndDecor();
	InitTextLabels();
	InitUIManager();
	InitSettingButton();
	InitAudioManager();

	m_Root->AddChild(m_Background);
	m_Root->AddChild(m_SettingButton);
	m_Root->AddChild(m_RedShawl);
	m_Root->AddChild(m_Title);
	m_Root->AddChild(m_Version);
	m_Root->AddChild(m_Text);

	FlushPendingObjectsToRendererAndCamera();
}

void MainMenuScene::Update()
{
	m_Root->Update();
	m_SettingButton->Update();
	// AudioManager::GetInstance().DrawDebugUI(); //測試用的
	UIManager::GetInstance().Update();
}

void MainMenuScene::Exit()
{
	LOG_DEBUG("Main Menu exited {}");
	AudioManager::GetInstance().PauseBGM();
}

Scene::SceneType MainMenuScene::Change()
{
	if (Util::Input::IsKeyDown(Util::Keycode::RETURN) || Util::Input::IsKeyDown(Util::Keycode::SPACE))
	{
		LOG_DEBUG("Change Lobby Scene");
		AudioManager::GetInstance().PlaySFX("click");
		return Scene::SceneType::Lobby;
	}
	return Scene::SceneType::Null;
}

void MainMenuScene::InitBackground() {
	auto& img = ImagePoolManager::GetInstance();
	m_Background->SetDrawable(img.GetImage(RESOURCE_DIR "/MainMenu/MainMenuBackground.png"));
	m_Background->SetZIndex(0);
}

void MainMenuScene::InitTitleAndDecor() {
	auto& img = ImagePoolManager::GetInstance();
	m_Title->SetDrawable(img.GetImage(RESOURCE_DIR "/MainMenu/Title.png"));
	m_Title->SetZIndex(2);
	m_Title->SetPivot({234, -221.5f});

	m_RedShawl->SetDrawable(img.GetImage(RESOURCE_DIR "/MainMenu/RedShawl.png"));
	m_RedShawl->SetZIndex(1);
	m_RedShawl->SetPivot({-237, -22.0f});
}

void MainMenuScene::InitTextLabels() {
	auto& img = ImagePoolManager::GetInstance();
	m_Text->SetDrawable(img.GetText(RESOURCE_DIR "/Font/zpix.ttf", 20, "ENTER開始OwOb", Util::Color(255, 255, 255)));
	m_Text->SetZIndex(2);
	m_Text->SetPivot({-11, 300});

	m_Version->SetDrawable(img.GetText(RESOURCE_DIR "/Font/zpix.ttf", 20, "版本號 v1.0.0", Util::Color(255, 255, 255)));
	m_Version->SetZIndex(2);
	m_Version->SetPivot({-450, 300});
}

void MainMenuScene::InitUIManager() {
	UIManager::GetInstance().ResetPanels();

	auto panel = std::make_shared<SettingPanel>();
	panel->Start();
	UIManager::GetInstance().RegisterPanel("setting", panel);
}

void MainMenuScene::InitSettingButton() {
	auto& img = ImagePoolManager::GetInstance();
	std::function<void()> onClick = []() {
		UIManager::GetInstance().ShowPanel("setting");
	};
	m_SettingButton = std::make_shared<UIButton>(onClick, false);
	m_SettingButton->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_settingPanel/button_setting.png"));
	m_SettingButton->SetZIndex(3.0f);
	m_SettingButton->m_Transform.translation = {-550.0f, -280.0f};
}

void MainMenuScene::InitAudioManager() {
	AudioManager::GetInstance().Reset();
	AudioManager::GetInstance().LoadFromJson("/Lobby/AudioConfig.json");
	AudioManager::GetInstance().PlayBGM();
}