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

	// 確保獲取場景數據
	if (!m_SceneData)
	{
		Scene::Download();
	}

	// 檢查數據是否成功獲取（Complete 場景可能不需要數據）
	LOG_DEBUG("Scene data status: {}", m_SceneData ? "Available" : "Not available");
}

void CompleteScene::Update() { LOG_DEBUG("Complete Scene is running..."); }

void CompleteScene::Exit() { LOG_DEBUG("Complete Scene exited"); }

Scene::SceneType CompleteScene::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}
