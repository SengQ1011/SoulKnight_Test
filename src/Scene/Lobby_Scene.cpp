//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/Lobby_Scene.hpp"

#include "GameMechanism/Talent.hpp"
#include "GameMechanism/TalentDatabase.hpp"

#include "Components/TalentComponet.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/InputComponent.hpp"

#include "Creature/Character.hpp"
#include "Factory/CharacterFactory.hpp"
#include "ObserveManager/InputManager.hpp"
#include "Room/LobbyRoom.hpp"
#include "Room/RoomCollisionManager.hpp"
#include "Util/Input.hpp"

// #include "Tracy.hpp"
#include "Tracy.hpp"
#include "Weapon/Weapon.hpp"

void LobbyScene::Start()
{

	// ZoneScopedN("LobbyScene::Start");
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

	// 初始化场景管理器
	InitializeSceneManagers();

	// 防止并行渲染器出事
	FlushPendingObjectsToRendererAndCamera();
}

void LobbyScene::Update()
{
	// Input处理
	// ZoneScopedN("LobbyScene::Update");
	if (Util::Input::IsKeyDown(Util::Keycode::B))
	{
		for (int i=0; i<10; i++) CreateEnemy();
	}
	{
		// ZoneScopedN("SceneManager::Update");
		for (auto& [type,manager]: m_Managers) manager->Update();
	}

	{
		// 更新房间
		// ZoneScopedN("LobbyRoom&RoomManager::Update");
		m_LobbyRoom->Update();
	}

	{
		// 更新相机
		// ZoneScopedN("Camera::Update");
		m_Camera->Update();
	}

	{
		// 更新渲染器
		// ZoneScopedN("Renderer::Update");
		m_Root->Update();
	}
}

void LobbyScene::CreatePlayer()
{
	// 使用 CharacterFactory 创建玩家
	m_Player = CharacterFactory::GetInstance().createPlayer(1);
	std::vector<Talent> talentDatabase = CreateTalentList();  // 創建天賦資料庫
	if(auto talentComp = m_Player->GetComponent<TalentComponent>(ComponentType::TALENT)){
		talentComp->AddTalent(talentDatabase[3]);
	}

	// 设置玩家的初始位置
	m_Player->SetWorldCoord(glm::vec2(-16*2, 16*2)); // 初始位置为右两格，上两格

	// 获取碰撞组件并添加到场景和相机
	auto collision = m_Player->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (collision) {
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
	LOG_DEBUG("Entering Lobby Scene1");
	m_Enemy = CharacterFactory::GetInstance().createEnemy(6);
	m_Enemy->m_WorldCoord = {32,16*2};
	LOG_DEBUG("Entering Lobby Scene2");
	auto collision2 = m_Enemy->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if(!collision2->GetVisibleBox()) LOG_ERROR("collision2->GetBlackBox()");
	const auto visibleBox = collision2->GetVisibleBox();
	if(!visibleBox) LOG_ERROR("collision2->GetBlackBox()");
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
	AddManager(ManagerTypes::ATTACK,std::make_shared<AttackManager>());

	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	// 注册输入观察者
	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);
}

void LobbyScene::Exit()
{
	LOG_DEBUG("Lobby Scene exited");
	// 退出场景时的清理工作
	m_BGM->Pause();
	if (m_LobbyRoom) {
		m_LobbyRoom->CharacterExit(std::dynamic_pointer_cast<Character>(m_Player));
	}
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
