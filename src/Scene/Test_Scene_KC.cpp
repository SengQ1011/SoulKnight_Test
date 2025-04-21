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

	// Input处理
	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	inputManager->Update();

	m_Player->Update();

	// 更新房间
	m_Map->Update();
	const std::shared_ptr<DungeonRoom> dungeonRoom = m_Map->GetCurrentRoom();
	if (dungeonRoom)
	{
		m_CurrentRoom = dungeonRoom;
		dungeonRoom->Update();
		// TODO: 之後寫到DungeonRoom cpp裏面用class包裝這裏呼叫
		const auto mark = dungeonRoom->GetMark();
		ImGui::Begin("Current Room Grid Viewer Can't Spawn");

		// if (ImGui::CollapsingHeader("Current Room Grid Viewer"))
		// {
			ImVec2 tableSize = ImGui::GetContentRegionAvail();
		ImGui::BeginChild("TableContainer", tableSize, false, ImGuiWindowFlags_None);
			if (ImGui::BeginTable("RoomTable", 35,
				ImGuiTableFlags_Borders |
				ImGuiTableFlags_Resizable |
				ImGuiTableFlags_SizingStretchProp))
			{
				for (int index = 0; index < 35; index++)
					ImGui::TableSetupColumn((std::to_string(index)).c_str(), ImGuiTableColumnFlags_WidthStretch, tableSize.x);

				ImGui::TableHeadersRow();

				for (int row = 0; row < 35; row++)
				{
					ImGui::TableNextRow();
					for (int col = 0; col < 35; col++)
					{
						ImGui::TableSetColumnIndex(col);
						if (mark[row][col])
						{
							ImGui::TextColored(ImVec4(0,0,1,1),"1");
						}
						else
						{
							ImGui::TextColored(ImVec4(1,1,1,1),"0");
						}
					}
				}
				ImGui::EndTable();
			}
		ImGui::EndChild();
		// }
		ImGui::End();

	}



	m_AttackManager->Update();
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



