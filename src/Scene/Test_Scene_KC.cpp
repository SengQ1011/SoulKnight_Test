//
// Created by QuzzS on 2025/3/4.
//
// ReSharper disable All
#include "Scene/Test_Scene_KC.hpp"

#include <functional>

#include "GameMechanism/Talent.hpp"
#include "GameMechanism/TalentDatabase.hpp"

#include "Components/CollisionComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Components/TalentComponet.hpp"

#include "Creature/Character.hpp"
#include "EnumTypes.hpp"
#include "Factory/CharacterFactory.hpp"
#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "ObserveManager/IManager.hpp"
#include "ObserveManager/InputManager.hpp"
#include "Room/LobbyRoom.hpp"
#include "Room/RoomCollisionManager.hpp"
#include "UIPanel/PausePanel.hpp"
#include "UIPanel/PlayerStatusPanel.hpp"
#include "UIPanel/SettingPanel.hpp"
#include "UIPanel/UIButton.hpp"
#include "UIPanel/UIManager.hpp"
#include "UIPanel/UIPanel.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"


#include "Weapon/Weapon.hpp"

// 簡單的回調函數用於暫停按鈕
void OpenPausePanel() { UIManager::GetInstance().ShowPanel("pause"); }

void TestScene_KC::Start()
{
	LOG_DEBUG("Entering KC Test Scene (with Lobby Settings)");
	// 创建并初始化玩家
	CreatePlayer();

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

	m_CurrentRoom = m_LobbyRoom;

	InitUIManager();
	InitPauseButton();
	InitAudioManager();

	// 初始化场景管理器
	InitializeSceneManagers();

	// 添加暫停按鈕到場景
	m_Root->AddChild(m_PauseButton);

	// 防止并行渲染器出事
	FlushPendingObjectsToRendererAndCamera();
}

void TestScene_KC::Update()
{
	// 先更新UI管理器，處理UI相關的輸入
	UIManager::GetInstance().Update();

	// 更新暫停按鈕
	m_PauseButton->Update();

	// 測試用：按P鍵顯示/隱藏暫停面板
	if (Util::Input::IsKeyDown(Util::Keycode::P))
	{
		UIManager::GetInstance().TogglePanel("pause");
	}

	// Input处理 - 只有當設定面板沒有顯示時才處理遊戲輸入
	if (!UIManager::GetInstance().IsPanelVisible("setting") && !UIManager::GetInstance().IsPanelVisible("pause"))
	{
		for (auto &[type, manager] : m_Managers)
			manager->Update();

		// 更新房间
		m_LobbyRoom->Update();
	}

	// 更新相机（相機總是需要更新）
	m_Camera->Update();

	// 更新渲染器（渲染總是需要更新）
	m_Root->Update();
}

void TestScene_KC::Exit()
{
	LOG_DEBUG("KC Test Scene exited");
	// 退出场景时的清理工作
	m_BGM->Pause();
	if (m_LobbyRoom)
	{
		m_LobbyRoom->CharacterExit(std::dynamic_pointer_cast<Character>(m_Player));
	}

	if (m_Player)
		SavePlayerInformation(m_Player);
}

Scene::SceneType TestScene_KC::Change()
{
	if (IsChange())
	{
		LOG_DEBUG("Change from KC Test Scene");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}

void TestScene_KC::CreatePlayer()
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

void TestScene_KC::SetupCamera() const
{
	m_Camera->SetMapSize(m_MapHeight);
	m_Camera->SetFollowTarget(m_Player);
}

void TestScene_KC::InitializeSceneManagers()
{
	// 添加管理器到场景
	AddManager(ManagerTypes::INPUT, std::static_pointer_cast<IManager>(std::make_shared<InputManager>()));
	AddManager(ManagerTypes::ATTACK, std::static_pointer_cast<IManager>(std::make_shared<AttackManager>()));

	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	// 注册输入观察者
	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);
}

void TestScene_KC::InitUIManager()
{
	UIManager::GetInstance().ResetPanels();

	const auto settingPanel = std::make_shared<SettingPanel>();
	settingPanel->Start();
	UIManager::GetInstance().RegisterPanel("setting", std::static_pointer_cast<UIPanel>(settingPanel));

	const auto pausePanel =
		std::make_shared<PausePanel>(m_Player->GetComponent<TalentComponent>(ComponentType::TALENT));
	pausePanel->Start();
	UIManager::GetInstance().RegisterPanel("pause", std::static_pointer_cast<UIPanel>(pausePanel));

	const auto playerStatusPanel =
		std::make_shared<PlayerStatusPanel>(m_Player->GetComponent<HealthComponent>(ComponentType::HEALTH));
	playerStatusPanel->Start();
	UIManager::GetInstance().RegisterPanel("playerStatus", std::static_pointer_cast<UIPanel>(playerStatusPanel));
}

void TestScene_KC::InitAudioManager()
{
	AudioManager::GetInstance().Reset();
	AudioManager::GetInstance().LoadFromJson("/Lobby/AudioConfig.json");
	AudioManager::GetInstance().PlayBGM();
}

void TestScene_KC::InitPauseButton()
{
	auto &img = ImagePoolManager::GetInstance();

	// 使用全局函數作為回調，顯示暫停面板
	m_PauseButton = std::make_shared<UIButton>(OpenPausePanel, false);
	m_PauseButton->SetDrawable(img.GetImage(RESOURCE_DIR "/UI/ui_pausePanel/button_pause.png"));
	m_PauseButton->SetZIndex(85.0f);
	m_PauseButton->m_Transform.translation = {580.0f, 310.0f};
}
