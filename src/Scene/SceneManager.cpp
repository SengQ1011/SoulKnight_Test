//
// Created by QuzzS on 2025/3/6.
//

#include "Scene/SceneManager.hpp"

#include "SaveManager.hpp"
#include "Scene/Complete_Scene.hpp"
#include "Scene/DungeonLoading_Scene.hpp"
#include "Scene/Dungeon_Scene.hpp"
#include "Scene/Lobby_Scene.hpp"
#include "Scene/MainMenu_Scene.hpp"
#include "Scene/Result_Scene.hpp"
#include "Scene/Test_Scene_JX.hpp"
#include "Scene/Test_Scene_KC.hpp"

SceneManager &SceneManager::GetInstance()
{
	static SceneManager instance;
	return instance;
}

void SceneManager::Start()
{
	// 初始化共享的場景數據
	auto &saveManager = SaveManager::GetInstance();
	if (saveManager.HasSaveData())
	{
		auto saveData = saveManager.GetSaveData();
		if (saveData)
		{
			m_Data = saveData;
		}
	}
	else
	{
		LOG_INFO("No saveData==>Init");
		InitializeNewGameData();
	}

	m_CurrentScene = CreateScene(Scene::SceneType::Test_KC);

	m_CurrentScene->Start();
}

void SceneManager::ChangeCurrentScene()
{
	// 優先檢查是否有外部設定的場景切換
	if (m_NextSceneType == Scene::SceneType::Null)
	{
		m_NextSceneType = m_CurrentScene->Change();
	}

	if (m_NextSceneType == Scene::SceneType::Null)
	{
		return;
	}

	// 退出舊場景
	m_CurrentScene->Exit();
	m_CurrentScene->Upload();
	m_CurrentScene = nullptr;

	// 載入新場景 特殊處理
	//  if (m_NextSceneType == Scene::SceneType::Dungeon)
	//  {
	//  	m_CurrentScene = DungeonScene::GetPreGenerated();
	//  	// DungeonScene::ClearPreGenerated();
	//  	m_CurrentScene->Start();
	//  }
	//  else
	//  {
	//  	m_CurrentScene = CreateScene(m_NextSceneType);
	//  	m_CurrentScene->Start();
	//  }
	m_CurrentScene = CreateScene(m_NextSceneType);
	// if (!m_Data->inDungeon && m_CurrentScene.Get == Scene::SceneType::DungeonLoad)

	m_CurrentScene->Download();
	m_CurrentScene->Start();

	// 重置下一個場景類型
	m_NextSceneType = Scene::SceneType::Null;
}

// 新增：允許外部直接設定下一個場景
void SceneManager::SetNextScene(Scene::SceneType nextScene) { m_NextSceneType = nextScene; }

std::shared_ptr<Scene> SceneManager::CreateScene(const Scene::SceneType type)
{
	switch (type)
	{
		// 確保type不會是Null
	case Scene::SceneType::Menu:
		return std::make_shared<MainMenuScene>();
	case Scene::SceneType::Test_KC:
		return std::make_shared<TestScene_KC>();
	case Scene::SceneType::Lobby:
		return std::make_shared<LobbyScene>();
	case Scene::SceneType::DungeonLoad:
		return std::make_shared<DungeonLoadingScene>(); // 關卡加載初始化后要先渲染一次畫面，方便關卡預先初始化
	case Scene::SceneType::Dungeon:
		return std::make_shared<DungeonScene>();
	case Scene::SceneType::Complete:
		return std::make_shared<CompleteScene>();
	case Scene::SceneType::Result:
		return std::make_shared<ResultScene>();
	default:
		LOG_ERROR("Unknown scene type In SceneManager");
		return std::make_shared<MainMenuScene>();
	}
}

void SceneManager::Update() const { m_CurrentScene->Update(); }

void SceneManager::End()
{
	m_Data = nullptr;
	m_CurrentScene->Exit();
	m_CurrentScene = nullptr;
}

std::shared_ptr<SaveData> SceneManager::DownloadGameProgress() const
{
	if (m_Data)
		return std::make_shared<SaveData>(*m_Data); // 深拷貝一份

	LOG_ERROR("No data");
	return nullptr;
}

void SceneManager::UploadGameProgress(std::shared_ptr<SaveData> &updatedProgress)
{
	// 更新自己的資料，並讓saveManager更新存檔
	if (updatedProgress)
	{
		m_Data = updatedProgress;
		SaveManager::GetInstance().SaveGame(m_Data);
	}
	else
	{
		LOG_ERROR("No data");
	}
}

void SceneManager::StartGame()
{
	if (const auto &saveManager = SaveManager::GetInstance(); saveManager.HasSaveData())
	{
		// 有存取資料就恢復
		if (m_Data->isInGameProgress)
		{
			m_CurrentScene = CreateScene(Scene::SceneType::Dungeon);
		}
		else
		{
			LOG_DEBUG("not in game");
			m_CurrentScene = CreateScene(Scene::SceneType::Lobby);
		}
		LOG_INFO("Game loaded successfully");
	}
	else
	{
		// 初始化新遊戲數據
		InitializeNewGameData();
		m_CurrentScene = CreateScene(Scene::SceneType::Lobby);
		LOG_INFO("Starting new game");
	}

	m_CurrentScene->Download();
	m_CurrentScene->Start();
}

void SceneManager::InitializeNewGameData()
{
	std::shared_ptr<SaveData> newData;
	// 直接初始化 SaveData 結構
	if (m_Data)
		newData = std::make_shared<SaveData>(*m_Data);
	else
		newData = std::make_shared<SaveData>();

	newData->saveName = "New Game";
	newData->saveTime = "";
	newData->isInGameProgress = false;

	// 初始化遊戲進度
	auto &progress = newData->gameProgress;
	progress.currentChapter = static_cast<int>(StageTheme::IcePlains);
	progress.currentStage = 0; // 在離開Lobby時開始更新
	progress.dungeonStartTime = 0; // 進入地牢再更新
	progress.cumulativeTime = 0;
	progress.killCount = 0;

	// 初始化玩家數據
	auto &playerData = progress.playerData;
	playerData.playerID = 1;
	playerData.currentHp = 0;
	playerData.currentEnergy = 0;
	playerData.money = 0;
	playerData.weaponID.clear();
	playerData.talentID.clear();

	UploadGameProgress(newData);
}

void SceneManager::receiveEnemyDeathEvent() const { m_CurrentScene->GetSaveData()->gameProgress.killCount++; }
