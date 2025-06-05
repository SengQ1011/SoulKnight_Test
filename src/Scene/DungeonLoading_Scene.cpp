//
// Created by QuzzS on 2025/3/4.
//


#include "Scene/DungeonLoading_Scene.hpp"
#include <thread>

#include "ImagePoolManager.hpp"
#include "Override/nGameObject.hpp"
#include "SaveManager.hpp"
#include "Scene/Dungeon_Scene.hpp"
#include "Util/Text.hpp"


void DungeonLoadingScene::Start()
{
	LOG_DEBUG("Entering Dungeon Loading Scene");

	// 確保獲取場景數據
	if (!m_SceneData)
	{
		Scene::Download();
	}

	// 檢查數據是否成功獲取
	if (!m_SceneData)
	{
		LOG_ERROR("Failed to get scene data in DungeonLoadingScene::Start()");
		return;
	}

	m_Text = std::make_shared<nGameObject>("DungeonLoadingScene");
	m_Text->SetDrawable(ImagePoolManager::GetInstance().GetText(RESOURCE_DIR "/Font/zpix.ttf", 20, "地牢加载中...",
																Util::Color(255, 255, 255)));
	m_Text->SetZIndex(2);
	m_Text->m_Transform.translation = glm::vec2(0);
	m_Root->AddChild(m_Text);
	m_Root->Update();
}

void DungeonLoadingScene::Update()
{
	// 檢查是否完成地牢生成
	// if (!m_DungeonReady && m_GenerationTask.valid() &&
	// 	m_GenerationTask.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	// {
	// 	m_DungeonReady = true;
	// 	LOG_DEBUG("[DungeonLoadingScene] Dungeon Ready for Transition");
	// }

	// 可加上 loading 動畫或顯示進度條
	m_Root->Update();
	// DungeonScene::GenerateStaticDungeon();
	m_DungeonReady = true;
}

void DungeonLoadingScene::Exit() { LOG_DEBUG("[DungeonLoadingScene] Exit"); }

Scene::SceneType DungeonLoadingScene::Change()
{
	if (m_DungeonReady)
	{
		// 已完成1-4了 ==> 進入boss關卡
		if (m_SceneData->gameProgress.currentStage == 4)
		{
			// return Scene::SceneType::DungeonBOSS;
			return Scene::SceneType::Dungeon;
		}
		// 已完成boss關卡 ==> 結算
		if (m_SceneData->gameProgress.currentStage == 5)
		{
			return Scene::SceneType::Result;
		}
		// otherwise
		return Scene::SceneType::Dungeon;
	}
	return Scene::SceneType::Null;
}
