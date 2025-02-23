//
// Created by QuzzS on 2025/2/23.
//

#include "Menu_Scene.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void MenuScene::Start()
{
	LOG_DEBUG("Entering Main Menu");
}

void MenuScene::Update()
{
	LOG_DEBUG("Main Menu is running...");

}

void MenuScene::Exit()
{
	LOG_DEBUG("Main Menu exited");
}

Scene::SceneType MenuScene::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		LOG_DEBUG("Change Game Scene");
		return Scene::SceneType::Game;
	}
	return Scene::SceneType::Null;
}





