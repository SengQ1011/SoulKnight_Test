//
// Created by QuzzS on 2025/2/23.
//

#include "MainMenu_Scene.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Image.hpp"

void MainMenuScene::Start()
{
	LOG_DEBUG("Entering Main Menu");
	m_Background->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/MainMenuBackground.png"));
	m_Background->SetZIndex(0);

	m_Root.AddChild(m_Background);
}

void MainMenuScene::Update()
{
	LOG_DEBUG("Main Menu is running...");
	m_Root.Update();
}

void MainMenuScene::Input()
{
	if (Util::Input::IsKeyDown(Util::Keycode::H))
	{
		LOG_DEBUG("MainMenu input \'H\' successfully detected");
	}
}


void MainMenuScene::Exit()
{
	LOG_DEBUG("Main Menu exited");
}

Scene::SceneType MainMenuScene::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		LOG_DEBUG("Change Game Scene");
		return Scene::SceneType::Game;
	}
	return Scene::SceneType::Null;
}





