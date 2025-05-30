//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/MainMenu_Scene.hpp"

#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Scene/SceneManager.hpp"
#include "Tool/Tool.hpp"
#include "UIPanel/SettingPanel.hpp"
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
	auto& imagePoolManager = ImagePoolManager::GetInstance();
	m_Background->SetDrawable(imagePoolManager.GetImage(RESOURCE_DIR"/MainMenu/MainMenuBackground.png"));
	m_Background->SetZIndex(0);

	m_Title->SetDrawable(imagePoolManager.GetImage(RESOURCE_DIR"/MainMenu/Title.png"));
	m_Title->SetZIndex(2);
	m_Title->SetPivot({234,-221.5});

	m_RedShawl->SetDrawable(imagePoolManager.GetImage(RESOURCE_DIR"/MainMenu/RedShawl.png"));
	m_RedShawl->SetZIndex(1);
	m_RedShawl->SetPivot({-237,-22});

	m_Text->SetDrawable(imagePoolManager.GetText(RESOURCE_DIR"/Font/zpix.ttf",20,"點擊開始OwOb",Util::Color(255,255,255)));
	m_Text->SetZIndex(2);
	m_Text->SetPivot({-11,300});

	m_Version->SetDrawable(imagePoolManager.GetText(RESOURCE_DIR"/Font/zpix.ttf",20,"版本號 v1.0.0",Util::Color(255,255,255)));
	m_Version->SetZIndex(2);
	m_Version->SetPivot({-450,300});

	std::shared_ptr<SettingPanel>settingPanel = std::make_shared<SettingPanel>();
	settingPanel->Start();
	UIManager::GetInstance().RegisterPanel("setting", settingPanel);

	AudioManager::GetInstance().Reset();
	AudioManager::GetInstance().LoadFromJson("/Lobby/AudioConfig.json");
	AudioManager::GetInstance().PlayBGM();

	m_Root->AddChild(m_Background);
	m_Root->AddChild(m_RedShawl);
	m_Root->AddChild(m_Title);
	m_Root->AddChild(m_Version);
	m_Root->AddChild(m_Text);
}

void MainMenuScene::Update()
{
	m_Root->Update();
	AudioManager::GetInstance().DrawDebugUI(); //測試用的
	UIManager::GetInstance().Update();
	if (Util::Input::IsKeyUp(Util::Keycode::T)) UIManager::GetInstance().TogglePanel("setting");
	if (Util::Input::IsKeyUp(Util::Keycode::O)) UIManager::GetInstance().ShowPanel("setting");
	if (Util::Input::IsKeyUp(Util::Keycode::H)) UIManager::GetInstance().HidePanel("setting");
}

void MainMenuScene::Exit()
{
	LOG_DEBUG("Main Menu exited {}");
}

Scene::SceneType MainMenuScene::Change()
{
	if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB) || Util::Input::IsKeyDown(Util::Keycode::RETURN) || Util::Input::IsKeyDown(Util::Keycode::SPACE))
	{
		AudioManager::GetInstance().PlaySFX("click");
		SceneManager::GetInstance().StartGame();
	}
	return Scene::SceneType::Null;
}

