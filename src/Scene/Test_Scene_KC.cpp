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

	// 初始化场景管理器
	InitializeSceneManagers();

	FlushPendingObjectsToRendererAndCamera();
}

void TestScene_KC::Update()
{

	// Input处理
	for (auto& [type,manager]: m_Managers) manager->Update();

	m_Player->Update();

	// 更新房间
	m_Map->Update();
	const std::shared_ptr<DungeonRoom> dungeonRoom = m_Map->GetCurrentRoom();
	if (dungeonRoom)
	{
		m_CurrentRoom = dungeonRoom;
		dungeonRoom->Update();

		dungeonRoom->DebugDungeonRoom();
	}

	// 更新相机
	m_Camera->Update();

	// 更新场景根节点
	m_Root->Update();

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
		// GetRoot().lock()->AddChild(collision->GetVisibleBox());
		// m_Camera->AddChild(collision->GetVisibleBox());
		m_PendingObjects.push_back(collision->GetVisibleBox());
	}

	// 将玩家添加到场景根节点和相机
	// GetRoot().lock()->AddChild(m_Player);
	// m_Camera->AddChild(m_Player);
	m_PendingObjects.push_back(m_Player);
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
	AddManager(ManagerTypes::ATTACK, std::make_shared<AttackManager>());

	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	// 注册输入观察者
	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);
}

void TestScene_KC::Exit()
{
	LOG_DEBUG("KC Test Scene exited");
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



