//
// Created by QuzzS on 2025/3/4.
//


#include "Scene/DungeonLoading_Scene.hpp"
#include <thread>

#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Override/nGameObject.hpp"
#include "SaveManager.hpp"
#include "Scene/Dungeon_Scene.hpp"
#include "UIPanel/TalentSelectionPanel.hpp"
#include "UIPanel/UIManager.hpp"
#include "Util/Logger.hpp"
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

	// 初始化天賦選擇面板並傳遞共享數據
	auto talentPanel = std::make_shared<TalentSelectionPanel>();
	talentPanel->SetSharedSaveData(m_SceneData);
	talentPanel->Start();
	UIManager::GetInstance().RegisterPanel("talent_selection", talentPanel, 100, true);
	if (talentPanel->IsVisible())
		UIManager::GetInstance().ShowPanel("talent_selection");

	FlushPendingObjectsToRendererAndCamera();
}

void DungeonLoadingScene::Update()
{
	// 更新UI管理器
	UIManager::GetInstance().Update();

	// 檢查天賦選擇是否完成
	if (!UIManager::GetInstance().IsPanelVisible("talent_selection"))
	{
		// 更新場景根節點
		// m_Root->Update();

		// 天賦選擇完成，設置地牢準備完成
		m_DungeonReady = true;
	}
	m_Root->Update();
}

void DungeonLoadingScene::Exit()
{
	LOG_DEBUG("[DungeonLoadingScene] Exit");
	// 清理天賦選擇面板
	UIManager::GetInstance().HidePanel("talent_selection");
}

Scene::SceneType DungeonLoadingScene::Change()
{
	if (m_DungeonReady)
	{
		// DungeonLoadingScene 只負責切換到 Dungeon_Scene
		LOG_DEBUG("DungeonLoadingScene: Loading complete, entering Dungeon");
		return Scene::SceneType::Dungeon;
	}
	return Scene::SceneType::Null;
}

