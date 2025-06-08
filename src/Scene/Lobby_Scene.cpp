//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/Lobby_Scene.hpp"

#include <functional>

#include "GameMechanism/Talent.hpp"
#include "GameMechanism/TalentDatabase.hpp"

#include "Components/CollisionComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Components/TalentComponet.hpp"
#include "Components/walletComponent.hpp"

#include "Creature/Character.hpp"
#include "Factory/CharacterFactory.hpp"
#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "ObserveManager/InputManager.hpp"
#include "Room/LobbyRoom.hpp"
#include "Room/RoomCollisionManager.hpp"
#include "UIPanel/GameHUDPanel.hpp"
#include "UIPanel/PausePanel.hpp"
#include "UIPanel/PlayerStatusPanel.hpp"
#include "UIPanel/SettingPanel.hpp"
#include "UIPanel/UIButton.hpp"
#include "UIPanel/UIManager.hpp"
#include "UIPanel/UIPanel.hpp"
#include "Util/Input.hpp"

#include "Weapon/Weapon.hpp"

void LobbyScene::Start()
{
	LOG_DEBUG("Entering Lobby Scene");

	// 確保獲取場景數據
	if (!m_SceneData)
	{
		Scene::Download();
	}

	// 檢查數據是否成功獲取
	if (!m_SceneData)
	{
		LOG_ERROR("Failed to get scene data in LobbyScene::Start()");
		return;
	}

	// 创建玩家角色
	CreatePlayer();
	CreateEnemy();

	// 设置相机
	m_MapHeight = 480.0f; // 大廳場景的地圖高度 480.0f
	SetupCamera();

	// 設置工廠
	m_RoomObjectFactory = std::make_shared<RoomObjectFactory>(m_Loader);

	// 创建并初始化大厅房间
	m_LobbyRoom = std::make_shared<LobbyRoom>(glm::vec2(0, 0), m_Loader, m_RoomObjectFactory);
	m_LobbyRoom->Start(m_Player);

	// 将玩家注册到碰撞管理器
	m_LobbyRoom->GetCollisionManager()->RegisterNGameObject(m_Player);
	// 将玩家添加到房间
	m_LobbyRoom->CharacterEnter(m_Player);
	m_LobbyRoom->CharacterEnter(m_Enemy);

	m_CurrentRoom = m_LobbyRoom;

	InitUIManager();
	InitAudioManager();

	// 初始化场景管理器
	InitializeSceneManagers();

	// 防止并行渲染器出事
	FlushPendingObjectsToRendererAndCamera();
}

void LobbyScene::Update()
{
	// 先更新UI管理器，處理UI相關的輸入
	UIManager::GetInstance().Update();

	// 測試用：按P鍵顯示/隱藏暫停面板
	if (Util::Input::IsKeyDown(Util::Keycode::P))
	{
		UIManager::GetInstance().TogglePanel("pause");
	}

	// Input处理 - 只有當設定面板和暫停面板都沒有顯示時才處理遊戲輸入
	if (!UIManager::GetInstance().IsPanelVisible("setting") && !UIManager::GetInstance().IsPanelVisible("pause"))
	{
		// Input处理
		if (Util::Input::IsKeyDown(Util::Keycode::B))
		{
			for (int i = 0; i < 10; i++)
				CreateEnemy();
		}

		if (Util::Input::IsKeyDown(Util::Keycode::T)) m_Camera->StartShake(0.3,5);

		for (auto &[type, manager] : m_Managers)
			manager->Update();

		// 更新房间
		m_LobbyRoom->Update();

		m_Camera->Update();
	}

	// 更新渲染器（渲染總是需要更新）
	m_Root->Update();
}

void LobbyScene::Exit()
{
	LOG_DEBUG("Lobby Scene exited");
	// 退出场景时的清理工作
	m_BGM->Pause();
	if (m_LobbyRoom)
	{
		m_LobbyRoom->CharacterExit(std::dynamic_pointer_cast<Character>(m_Player));
	}

	if (m_Player)
		SavePlayerInformation(m_Player);
}

Scene::SceneType LobbyScene::Change()
{
	if (IsChange())
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::DungeonLoad;
	}
	return Scene::SceneType::Null;
}

void LobbyScene::CreatePlayer()
{
	// 使用 CharacterFactory 创建玩家
	m_Player = CharacterFactory::GetInstance().createPlayer(1);
	std::vector<Talent> talentDatabase = CreateTalentList(); // 創建天賦資料庫
	if (auto talentComp = m_Player->GetComponent<TalentComponent>(ComponentType::TALENT))
	{
		talentComp->AddTalent(talentDatabase[3]);
	}

	// 设置玩家的初始位置
	m_Player->SetWorldCoord(glm::vec2(-16 * 2, 16 * 2)); // 初始位置为右两格，上两格

	// 获取碰撞组件并添加到场景和相机
	auto collision = m_Player->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (collision)
	{
		// 将碰撞盒添加到场景根节点和相机
		const auto visibleBox = collision->GetVisibleBox();
		m_PendingObjects.push_back(visibleBox);
		visibleBox->SetRegisteredToScene(true);
	}

	// 将玩家添加到场景根节点和相机
	m_PendingObjects.push_back(m_Player);
	m_Player->SetRegisteredToScene(true);
}

void LobbyScene::CreateEnemy()
{
	m_Enemy = CharacterFactory::GetInstance().createEnemy(21);
	m_Enemy->m_WorldCoord = {32,16*2};
	auto collision2 = m_Enemy->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (!collision2->GetVisibleBox())
		LOG_ERROR("collision2->GetBlackBox()");
	const auto visibleBox = collision2->GetVisibleBox();
	if (!visibleBox)
		LOG_ERROR("collision2->GetBlackBox()");
	m_PendingObjects.push_back(visibleBox);
	visibleBox->SetRegisteredToScene(true);
	m_PendingObjects.push_back(m_Enemy);
	m_Enemy->SetRegisteredToScene(true);
}

void LobbyScene::SetupCamera() const
{
	m_Camera->SetMapSize(m_MapHeight);
	m_Camera->SetFollowTarget(m_Player);
}

void LobbyScene::InitializeSceneManagers()
{
	// 添加管理器到场景
	AddManager(ManagerTypes::INPUT, std::make_shared<InputManager>());
	AddManager(ManagerTypes::ATTACK, std::make_shared<AttackManager>());

	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	// 注册输入观察者
	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);
}

void LobbyScene::InitUIManager()
{
	UIManager::GetInstance().ResetPanels();

	// 創建設定面板 - 最高優先級模態面板
	const auto settingPanel = std::make_shared<SettingPanel>();
	settingPanel->Start();
	UIManager::GetInstance().RegisterPanel("setting", std::static_pointer_cast<UIPanel>(settingPanel), 2, true);

	// 創建暫停面板 - 中等優先級模態面板
	const auto pausePanel =
		std::make_shared<PausePanel>(m_Player->GetComponent<TalentComponent>(ComponentType::TALENT));
	pausePanel->Start();
	UIManager::GetInstance().RegisterPanel("pause", std::static_pointer_cast<UIPanel>(pausePanel), 1, true);

	// 創建遊戲 HUD 面板 - 低優先級非模態面板
	const auto gameHUDPanel =
		std::make_shared<GameHUDPanel>(m_Player->GetComponent<HealthComponent>(ComponentType::HEALTH),
									   m_Player->GetComponent<WalletComponent>(ComponentType::WALLET));
	gameHUDPanel->Start();
	UIManager::GetInstance().RegisterPanel("gameHUD", std::static_pointer_cast<UIPanel>(gameHUDPanel), 0, false);
}

void LobbyScene::InitAudioManager()
{
	AudioManager::GetInstance().Reset();
	AudioManager::GetInstance().LoadFromJson("/Lobby/AudioConfig.json");
	AudioManager::GetInstance().PlayBGM();
}