//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/Result_Scene.hpp"

#include "SaveManager.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void ResultScene::Start()
{
	LOG_INFO("Entering Result Scene");
	// 關卡完成，結算分數
	const int totalSecond = m_SceneData->gameProgress.cumulativeTime / 1000;
	const int timeScore = std::max(0, 1939 - 2 * totalSecond);
	const int killScore = m_SceneData->gameProgress.killCount * 2;
	const int totalScore = timeScore + killScore;
	const int gameMoney = totalScore / 100;

	// 保存進度
	m_SceneData->saveName = "Progress Save";
	m_SceneData->isInGameProgress = false; // 關卡完成，回到Lobby
	m_SceneData->gameData.gameMoney = gameMoney;
	Upload();
}

void ResultScene::Update()
{
	LOG_DEBUG("Result Scene is running...");
}

void ResultScene::Exit()
{
	LOG_DEBUG("Result Scene exited");
	// 清空game
}

Scene::SceneType ResultScene::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		LOG_DEBUG("Change to Lobby");
		return Scene::SceneType::Lobby;
	}
	return Scene::SceneType::Null;
}