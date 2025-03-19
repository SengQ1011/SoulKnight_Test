//
// Created by QuzzS on 2025/3/6.
//

#include "Scene/SceneManager.hpp"
void SceneManager::Start()
{
	m_CurrentScene = CreateScene(Scene::SceneType::Test_KC);
	m_CurrentScene->Start();
	m_CurrentScene->Download(m_Data);
}

void SceneManager::ChangeCurrentScene()
{
	//場景切換 - 主菜單、游戲局内畫面
	m_NextSceneType = m_CurrentScene->Change();
	if (m_NextSceneType == Scene::SceneType::Null)
	{
		return;
	}

	//退出舊場景
	m_Data = m_CurrentScene->Upload();
	m_CurrentScene->Exit();

	//載入新場景 特殊處理
	if (m_NextSceneType == Scene::SceneType::DungeonLoad)
	{
		m_CurrentScene = CreateScene(m_NextSceneType);
		m_PreLoadScene = CreateScene(Scene::SceneType::Dungeon);

		m_CurrentScene->Start();
		m_PreLoadScene->Start();
	}
	else if (m_NextSceneType == Scene::SceneType::Dungeon)
	{
		m_CurrentScene = m_PreLoadScene;
		m_PreLoadScene = nullptr;
	}
	else
	{
		m_CurrentScene = CreateScene(m_NextSceneType);
		m_CurrentScene->Start();
	}
	m_CurrentScene->Download(m_Data);
}

std::shared_ptr<Scene> SceneManager::CreateScene(Scene::SceneType type)
{
	switch (type)
	{
		//確保type不會是Null
	case Scene::SceneType::Menu:
		return std::make_shared<MainMenuScene>();
	case Scene::SceneType::Test_KC:
		return std::make_shared<TestScene_KC>();
	case Scene::SceneType::Test_JX:
		return std::make_shared<TestScene_JX>();
	case Scene::SceneType::Lobby:
		return std::make_shared<LobbyScene>();
	case Scene::SceneType::DungeonLoad:
		return std::make_shared<DungeonLoadingScene>(); //關卡加載初始化后要先渲染一次畫面，方便關卡預先初始化
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

void SceneManager::Update() const
{
	m_CurrentScene->Input();
	m_CurrentScene->Update();
}

void SceneManager::End()
{
	m_Data = nullptr;
	m_CurrentScene->Exit();
	m_CurrentScene = nullptr;
	m_PreLoadScene = nullptr;
}




