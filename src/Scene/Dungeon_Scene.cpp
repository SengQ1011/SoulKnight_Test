//
// Created by QuzzS on 2025/3/4.
//


#include "Scene/Dungeon_Scene.hpp"

#include "Components/CollisionComponent.hpp"
#include "Components/FollowerComponent.hpp"

#include "Components/InteractableComponent.hpp"
#include "Cursor.hpp"
#include "Loader.hpp"
#include "Scene/SceneManager.hpp"
#include "ObserveManager/InputManager.hpp"
#include "Attack/AttackManager.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include <iostream>

#include "Components/InputComponent.hpp"
#include "Creature/Character.hpp"
#include "Factory/CharacterFactory.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Room/DungeonMap.hpp"

std::shared_ptr<DungeonScene> DungeonScene::s_PreGeneratedInstance = nullptr;

void DungeonScene::Start()
{
	LOG_DEBUG("Entering Game Scene");
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

void DungeonScene::Update()
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

void DungeonScene::Exit()
{
	LOG_DEBUG("Game Scene exited");
}

Scene::SceneType DungeonScene::Change()
{
	if (m_IsChange)
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}

void DungeonScene::GenerateStaticDungeon() {
	s_PreGeneratedInstance = std::make_shared<DungeonScene>();
	s_PreGeneratedInstance->BuildDungeon(); // 真正重的初始化過程
}

std::shared_ptr<DungeonScene> DungeonScene::GetPreGenerated() {
	if (!s_PreGeneratedInstance) {
		LOG_ERROR("s_PreGeneratedInstance is null! You forgot to call GenerateStaticDungeon?");
	}
	return s_PreGeneratedInstance;
}

void DungeonScene::ClearPreGenerated() { s_PreGeneratedInstance = nullptr; }

void DungeonScene::BuildDungeon()
{
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
}

void DungeonScene::CreatePlayer()
{
	// 使用 CharacterFactory 创建玩家
	m_Player = CharacterFactory::GetInstance().createPlayer(1);

	// 设置玩家的初始位置
	m_Player->SetWorldCoord(glm::vec2(0)); // 地图正中央

	// 获取碰撞组件并添加到场景和相机
	if (const auto collision = m_Player->GetComponent<CollisionComponent>(ComponentType::COLLISION)) {
		// 将碰撞盒添加到场景根节点和相机
		m_PendingObjects.emplace_back(collision->GetVisibleBox());
		// m_Camera->AddChild(collision->GetVisibleBox());
	}

	// 将玩家添加到场景根节点和相机
	m_PendingObjects.emplace_back(m_Player);
	// m_Camera->AddChild(m_Player);
}

void DungeonScene::SetupCamera() const
{
	m_Camera->SetMapSize(m_MapHeight);
	m_Camera->SetFollowTarget(m_Player);
}

void DungeonScene::InitializeSceneManagers()
{
	// 添加管理器到场景
	AddManager(ManagerTypes::INPUT, std::make_shared<InputManager>());
	AddManager(ManagerTypes::ATTACK, std::make_shared<AttackManager>());

	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	// 注册输入观察者
	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);
}




