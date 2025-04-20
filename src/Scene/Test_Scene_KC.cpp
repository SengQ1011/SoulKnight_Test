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
#include "ObserveManager/InputManager.hpp"
#include "pch.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include <execution>
#include <iostream>

#include "Components/InputComponent.hpp"
#include "Creature/Character.hpp"
#include "Factory/CharacterFactory.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Room/DungeonMap.hpp"

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

	m_Map = std::make_shared<DungeonMap>(m_RoomObjectFactory,m_Loader,m_Player);
	m_Map->Start();
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

	// 初始化场景管理器
	InitializeSceneManagers();
}

void TestScene_KC::Update()
{
	auto time1 = std::chrono::high_resolution_clock::now();
	// Input处理
	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	inputManager->Update();
	auto time2 = std::chrono::high_resolution_clock::now();

	m_Player->Update();
	auto time3 = std::chrono::high_resolution_clock::now();

	// 更新房间
	m_Map->Update();
	auto time4 = std::chrono::high_resolution_clock::now();
	m_CurrentRoom = m_Map->GetCurrentRoom();
	auto time5 = std::chrono::high_resolution_clock::now();
	if (m_CurrentRoom) m_CurrentRoom->Update();

	auto time6 = std::chrono::high_resolution_clock::now();

	m_AttackManager->Update();
	auto time7 = std::chrono::high_resolution_clock::now();
	// 更新相机
	m_Camera->Update();
	auto time8 = std::chrono::high_resolution_clock::now();

	// 更新场景根节点
	m_Root->Update();
	auto time9 = std::chrono::high_resolution_clock::now();
	// 计算所有区块的耗时
	auto d_input    = std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1).count();
	auto d_player   = std::chrono::duration_cast<std::chrono::microseconds>(time3 - time2).count();
	auto d_map      = std::chrono::duration_cast<std::chrono::microseconds>(time4 - time3).count();
	auto d_roomGet  = std::chrono::duration_cast<std::chrono::microseconds>(time5 - time4).count();
	auto d_roomUpd  = std::chrono::duration_cast<std::chrono::microseconds>(time6 - time5).count();
	auto d_attack   = std::chrono::duration_cast<std::chrono::microseconds>(time7 - time6).count();
	auto d_camera   = std::chrono::duration_cast<std::chrono::microseconds>(time8 - time7).count();
	auto d_root     = std::chrono::duration_cast<std::chrono::microseconds>(time9 - time8).count();
	auto d_total    = std::chrono::duration_cast<std::chrono::microseconds>(time9 - time1).count();

	// 一次性输出
	std::cout << "[Time(us)] Input: " << d_input
			  << ", Player: " << d_player
			  << ", Map: " << d_map
			  << ", GetRoom: " << d_roomGet
			  << ", RoomUpdate: " << d_roomUpd
			  << ", Attack: " << d_attack
			  << ", Camera: " << d_camera
			  << ", Root: " << d_root
			  << " | Total: " << d_total
			  << std::endl;
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



