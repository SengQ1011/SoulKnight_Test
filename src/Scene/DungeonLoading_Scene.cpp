//
// Created by QuzzS on 2025/3/4.
//


#include "Scene/DungeonLoading_Scene.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void DungeonLoadingScene::Start()
{
	LOG_DEBUG("Entering Dungeon Loading Scene");
}

void DungeonLoadingScene::Update()
{
	LOG_DEBUG("Dungeon Loading Scene is running...");
}

void DungeonLoadingScene::Exit()
{
	LOG_DEBUG("Dungeon Loading Scene exited");
}

Scene::SceneType DungeonLoadingScene::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}

