//
// Created by QuzzS on 2025/3/4.
//


#include "Scene/Dungeon_Scene.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void DungeonScene::Start()
{
	LOG_DEBUG("Entering Game Scene");
}

void DungeonScene::Update()
{
	LOG_DEBUG("Game Scene is running...");
}

void DungeonScene::Exit()
{
	LOG_DEBUG("Game Scene exited");
}

Scene::SceneType DungeonScene::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}
