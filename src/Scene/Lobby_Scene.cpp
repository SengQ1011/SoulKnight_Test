//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/Test_Scene_KC.hpp"

#include "Components/TalentComponet.hpp"
#include "GameMechanism/Talent.hpp"
#include "GameMechanism/TalentDatabase.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/FollowerComponent.hpp"

#include "Scene/SceneManager.hpp"
#include "pch.hpp"

#include "Util/Logger.hpp"


void LobbyScene::Start()
{
	LOG_DEBUG("Entering Lobby Scene");
	// 创建并初始化玩家
	CreatePlayer();
	CreateEnemy();

	// 设置相机
	m_MapHeight = 480.0f ; //大廳場景的地圖高度 480.0f
	SetupCamera();

	//設置工廠
	m_RoomObjectFactory = std::make_shared<RoomObjectFactory>(m_Loader);

	// 创建并初始化大厅房间
	m_LobbyRoom = std::make_shared<LobbyRoom>(glm::vec2(0,0),m_Loader,m_RoomObjectFactory);
	m_LobbyRoom->Start(m_Player);

	// 将玩家注册到碰撞管理器
	m_LobbyRoom->GetCollisionManager()->RegisterNGameObject(m_Player);
	// 将玩家添加到房间
	m_LobbyRoom->CharacterEnter(m_Player);
	m_LobbyRoom->CharacterEnter(m_Enemy);

	m_CurrentRoom = m_LobbyRoom;

	m_trackingManager->AddTerrainObjects(m_LobbyRoom->GetRoomObjects());
	m_trackingManager->AddTerrainObjects(m_LobbyRoom->GetWallColliders());

	// 初始化场景管理器
	InitializeSceneManagers();
}

void LobbyScene::Update()
{
	// Input处理
	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	inputManager->Update();
	m_trackingManager->Update();

	m_Player->Update();
	m_Enemy->Update();

	// 更新房间
	m_LobbyRoom->Update();
	m_AttackManager->Update();

	// 更新相机
	m_Camera->Update();

	// 更新场景根节点
	m_Root->Update();
}

void LobbyScene::CreatePlayer()
{
	// 使用 CharacterFactory 创建玩家
	m_Player = CharacterFactory::GetInstance().createPlayer(1);
	std::vector<Talent> talentDatabase = CreateTalentList();  // 創建天賦資料庫
	if(auto talentComp = m_Player->GetComponent<TalentComponent>(ComponentType::TALENT)){
		talentComp->AddTalent(talentDatabase[2]);
	}

	// 设置玩家的初始位置
	m_Player->SetWorldCoord(glm::vec2(0, 16*2)); // 初始位置为右两格，上两格

	// 获取碰撞组件并添加到场景和相机
	auto collision = m_Player->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (collision) {
		// 将碰撞盒添加到场景根节点和相机
		GetRoot().lock()->AddChild(collision->GetVisibleBox());
		m_Camera->AddChild(collision->GetVisibleBox());
	}

	m_trackingManager->SetPlayer(m_Player);
	// 将玩家添加到场景根节点和相机
	GetRoot().lock()->AddChild(m_Player);
	m_Camera->AddChild(m_Player);
}

void LobbyScene::CreateEnemy()
{
	m_Enemy = CharacterFactory::GetInstance().createEnemy(1);
	m_Enemy->m_WorldCoord = {32,16*2};
	auto collision2 = m_Enemy->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if(!collision2->GetVisibleBox())LOG_ERROR("collision2->GetBlackBox()");
	m_Root->AddChild(collision2->GetVisibleBox());
	m_Camera->AddChild(collision2->GetVisibleBox());
	m_trackingManager->AddEnemy(m_Enemy);
	m_Root->AddChild(m_Enemy);
	m_Camera->AddChild(m_Enemy);
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
	AddManager(ManagerTypes::ROOMCOLLISION, m_LobbyRoom->GetCollisionManager());
	// AddManager(ManagerTypes::ATTACK,m_LobbyRoom->GetAttackManager());
	AddManager(ManagerTypes::ATTACK,m_AttackManager);
	AddManager(ManagerTypes::TRACKING, m_trackingManager);

	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	// 注册输入观察者
	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);
}

void LobbyScene::Exit()
{
	LOG_DEBUG("Lobby Scene exited");
	// 退出场景时的清理工作
	if (m_LobbyRoom) {
		m_LobbyRoom->CharacterExit(std::dynamic_pointer_cast<Character>(m_Player));
	}
}

Scene::SceneType LobbyScene::Change()
{
	if (IsChange())
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Test_KC;
	}
	return Scene::SceneType::Null;
}
