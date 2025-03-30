//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/Result_Scene.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void ResultScene::Start()
{
	LOG_DEBUG("Entering Result Scene");
}

void ResultScene::Update()
{
	LOG_DEBUG("Result Scene is running...");
}

void ResultScene::Exit()
{
	LOG_DEBUG("Result Scene exited");
}

Scene::SceneType ResultScene::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}
