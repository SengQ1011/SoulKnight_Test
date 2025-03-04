//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/Game_Scene.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void GameScene::Start()
{
	LOG_DEBUG("Entering Game Scene");
}

void GameScene::Input()
{
	if (Util::Input::IsKeyDown(Util::Keycode::H))
	{
		LOG_DEBUG("GameScene input \'H\' successfully detected");
	}
}

void GameScene::Update()
{
	LOG_DEBUG("Game Scene is running...");
}

void GameScene::Exit()
{
	LOG_DEBUG("Game Scene exited");
}

Scene::SceneType GameScene::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}
