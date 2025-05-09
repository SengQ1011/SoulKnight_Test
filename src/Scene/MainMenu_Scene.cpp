//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/MainMenu_Scene.hpp"

#include "ObserveManager/AudioManager.hpp"
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
	m_Background->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/MainMenuBackground.png"));
	m_Background->SetZIndex(0);

	m_Title->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/Title.png"));
	m_Title->SetZIndex(2);
	m_Title->SetPivot({234,-221.5});

	m_RedShawl->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/RedShawl.png"));
	m_RedShawl->SetZIndex(1);
	m_RedShawl->SetPivot({-237,-22});

	m_Text->SetDrawable(std::make_shared<Util::Text>(RESOURCE_DIR"/Font/zpix.ttf",20,"點擊開始OwOb",Util::Color(255,255,255)));
	m_Text->SetZIndex(2);
	m_Text->SetPivot({-11,300});

	m_Version->SetDrawable(std::make_shared<Util::Text>(RESOURCE_DIR"/Font/zpix.ttf",20,"版本號 v1.0.0",Util::Color(255,255,255)));
	m_Version->SetZIndex(2);
	m_Version->SetPivot({-450,300});

	// m_MainMenu->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/UI/MenuPanel.png"));
	// m_MainMenu->SetZIndex(100);
	// m_MainMenu->m_Transform.translation = glm::vec2(140,80);
	// std::shared_ptr<SettingPanel>settingPanel = std::make_shared<SettingPanel>();
	// settingPanel->Start();
	// UIManager::GetInstance().RegisterPanel("setting", settingPanel);


	// m_BGM->FadeIn(1);
	AudioManager::GetInstance().Reset();
	AudioManager::GetInstance().LoadFromJson("/Lobby/AudioConfig.json");
	AudioManager::GetInstance().PlayBGM();

	m_Root->AddChild(m_Background);
	m_Root->AddChild(m_RedShawl);
	m_Root->AddChild(m_Title);
	m_Root->AddChild(m_Version);
	m_Root->AddChild(m_Text);
	// m_Root->AddChild(m_MainMenu);
}

void MainMenuScene::Update()
{
	m_Root->Update();
	AudioManager::GetInstance().DrawDebugUI(); //測試用的
	UIManager::GetInstance().Update();
	// if (Util::Input::IsKeyUp(Util::Keycode::T)) UIManager::GetInstance().TogglePanel("setting");
	// if (Util::Input::IsKeyUp(Util::Keycode::O)) UIManager::GetInstance().ShowPanel("setting");
	// if (Util::Input::IsKeyUp(Util::Keycode::H)) UIManager::GetInstance().HidePanel("setting");
}

void MainMenuScene::Exit()
{
	LOG_DEBUG("Main Menu exited {}");
}

Scene::SceneType MainMenuScene::Change()
{
	if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB) || Util::Input::IsKeyDown(Util::Keycode::RETURN) || Util::Input::IsKeyDown(Util::Keycode::SPACE))
	{
		LOG_DEBUG("Change Lobby Scene");
		AudioManager::GetInstance().PlaySFX("click");
		return Scene::SceneType::Lobby;
	}
	return Scene::SceneType::Null;
}

