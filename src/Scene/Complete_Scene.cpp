//
// Created by QuzzS on 2025/3/4.
//


#include "Scene/Complete_Scene.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void CompleteScene::Start()
{
	LOG_DEBUG("Entering Complete Scene");
}

void CompleteScene::Input()
{
	if (Util::Input::IsKeyDown(Util::Keycode::H))
	{
		LOG_DEBUG("CompleteScene input \'H\' successfully detected");
	}
}

void CompleteScene::Update()
{
	LOG_DEBUG("Complete Scene is running...");
}

void CompleteScene::Exit()
{
	LOG_DEBUG("Complete Scene exited");
}

Scene::SceneType CompleteScene::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}

