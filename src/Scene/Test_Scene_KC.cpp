//
// Created by QuzzS on 2025/3/4.
//
// ReSharper disable All
#include "Scene/Test_Scene_KC.hpp"

#include "Components/CollisionComponent.hpp"
#include "Components/FollowerComponent.hpp"

#include "Components/InteractableComponent.hpp"
#include "Cursor.hpp"
#include "Scene/SceneManager.hpp"
#include "pch.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include <execution>

void TestScene_KC::Start()
{
	LOG_DEBUG("Entering KC Test Scene");
	m_Loader = std::make_shared<Loader>(m_ThemeName);

	// 创建并初始化玩家
	CreatePlayer();

	// 设置相机
	m_MapHeight = 5 * 35 * 16 ; //Dungeon 5個房間 35個方塊 16像素
	SetupCamera();

	//設置工廠
	m_RoomObjectFactory = std::make_shared<RoomObjectFactory>(m_Loader);

	// // 创建并初始化大厅房间
	// m_DungeonRoom = std::make_shared<DungeonRoom>(glm::vec2(0,0),m_Loader,m_RoomObjectFactory);
	// m_DungeonRoom->Start(m_Player);
	//
	// m_RoomObjectGroup = m_DungeonRoom->GetRoomObjects();
	//
	// // 将玩家注册到碰撞管理器
	// m_DungeonRoom->GetCollisionManager()->RegisterNGameObject(m_Player);
	// // 将玩家添加到房间
	// m_DungeonRoom->CharacterEnter(m_Player);
	float tileSize = 16;
	float roomRegion = 35;
	float mapSizeInGrid = 5;
	auto startPos = glm::vec2(std::floor(mapSizeInGrid/2) * roomRegion * tileSize);
	startPos *= glm::vec2(-1,1);
	float offsetRoom = tileSize * roomRegion;

	std::vector<int> indices(static_cast<int>(m_DungeonRooms.size()));
	std::iota(indices.begin(), indices.end(), 0);

	std::vector<std::shared_ptr<DungeonRoom>> localRooms(m_DungeonRooms.size());

	std::for_each(std::execution::par, indices.begin(), indices.end(), [&](int i)
	{
		auto x = i % static_cast<int>(mapSizeInGrid);
		auto y = i / static_cast<int>(mapSizeInGrid);

		glm::vec2 roomPosition = startPos + glm::vec2(offsetRoom, -offsetRoom) * glm::vec2(x, y);
		auto room = std::make_shared<DungeonRoom>(roomPosition,m_Loader,m_RoomObjectFactory);
		localRooms[i] = room;
	});
	for (int i=0; i<localRooms.size(); ++i)
	{
		m_DungeonRooms[i] = localRooms[i];
		m_DungeonRooms[i]->Start(m_Player);
		m_DungeonRooms[i]->CharacterEnter(m_Player);
	}
	// 初始化场景管理器
	InitializeSceneManagers();
}

void TestScene_KC::Update()
{
	// auto time1 = std::chrono::high_resolution_clock::now();
	// Input处理
	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	// auto time2 = std::chrono::high_resolution_clock::now();
	inputManager->Update();
	// auto time3 = std::chrono::high_resolution_clock::now();

	m_Player->Update();
	// auto time4 = std::chrono::high_resolution_clock::now();

	// 更新房间
	// auto time5 = std::chrono::high_resolution_clock::now();
	for(int i=0; i<m_DungeonRooms.size(); i++)
	{
		if (!m_DungeonRooms[i]->IsPlayerInside()) continue;
		m_CurrentRoom = m_DungeonRooms[i];
		//LOG_DEBUG("DungeonRoom {} {}", i%5, i/5);
	}
	m_CurrentRoom->Update();
	m_AttackManager->Update();
	// std::for_each(m_DungeonRooms.begin(), m_DungeonRooms.end(), [](const std::shared_ptr<DungeonRoom>& room){room->Update();});
	// auto time6 = std::chrono::high_resolution_clock::now();
	// 更新相机
	m_Camera->Update();
	// auto time7 = std::chrono::high_resolution_clock::now();

	// 更新场景根节点
	GetRoot().lock()->Update();
	// auto time8 = std::chrono::high_resolution_clock::now();
	// auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1);
	// auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(time3 - time2);
	// auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(time4 - time3);
	// auto duration4 = std::chrono::duration_cast<std::chrono::milliseconds>(time5 - time4);
	// auto duration5 = std::chrono::duration_cast<std::chrono::milliseconds>(time6 - time5);
	// auto duration6 = std::chrono::duration_cast<std::chrono::milliseconds>(time7 - time6);
	// auto duration7 = std::chrono::duration_cast<std::chrono::milliseconds>(time8 - time7);
	// auto time9 = std::chrono::high_resolution_clock::now();
	// auto duration8 = std::chrono::duration_cast<std::chrono::microseconds>(time9 - time1);
	// LOG_DEBUG("Update useTime End:{}", duration8);
	// LOG_DEBUG("Update useTime GetInputManager:{}, ManagerUpdate:{}, PlayerUpdate:{}, RoomUpdate:{}, CameraUpdate{}, RendererUpdate{}, End:{}",
	// 	duration1, duration2, duration3, duration4, duration5, duration6, duration7, duration8);
}

void TestScene_KC::CreatePlayer()
{
	// 使用 CharacterFactory 创建玩家
	m_Player = CharacterFactory::GetInstance().createPlayer(1);

	// 设置玩家的初始位置
	m_Player->SetWorldCoord(glm::vec2(0, 16*2)); // 初始位置为右两格，上两格

	// 获取碰撞组件并添加到场景和相机
	auto collision = m_Player->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (collision) {
		// 将碰撞盒添加到场景根节点和相机
		GetRoot().lock()->AddChild(collision->GetVisibleBox());
		m_Camera->AddChild(collision->GetVisibleBox());
	}

	// 将玩家添加到场景根节点和相机
	GetRoot().lock()->AddChild(m_Player);
	m_Camera->AddChild(m_Player);
}

void TestScene_KC::SetupCamera() const
{
	m_Camera->SetMapSize(m_MapHeight);
	m_Camera->SetFollowTarget(m_Player);
}

void TestScene_KC::InitializeSceneManagers()
{
	// 添加管理器到场景
	AddManager(ManagerTypes::INPUT, std::make_shared<InputManager>());
	AddManager(ManagerTypes::ATTACK, m_AttackManager);
	// AddManager(ManagerTypes::ROOMCOLLISION, m_DungeonRoom->GetCollisionManager());

	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	// 注册输入观察者
	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);
}

void TestScene_KC::Exit()
{
	LOG_DEBUG("KC Test Scene exited");
	// 退出场景时的清理工作
	// if (m_DungeonRoom) {
	// 	m_DungeonRoom->CharacterExit(std::dynamic_pointer_cast<Character>(m_Player));
	// }
}

Scene::SceneType TestScene_KC::Change()
{
	if (IsChange())
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}



