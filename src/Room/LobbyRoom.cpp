//
// Created by QuzzS on 2025/4/5.
//

#include "Room/LobbyRoom.hpp"

#include <iostream>
#include "Components/CollisionComponent.hpp"
#include "Creature/Character.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Loader.hpp"
#include "Util/Logger.hpp"
#include "Scene/SceneManager.hpp"


void LobbyRoom::Start(const std::shared_ptr<Character> &player)
{
	LOG_DEBUG("Initial LobbyRoom start");

	// 调用基类的Start方法
	Room::Start(player);

	// 設置大廳特有的墻壁碰撞體
	SetupWallColliders();
}

void LobbyRoom::Update()
{
	// 调用基类Update
	Room::Update();

	// 簡單的防漏洞機制：檢測玩家是否在房間區域內
	ValidatePlayerPosition();

	// 更新所有墙壁碰撞体
	// for (auto& wall : m_WallColliders) {
	// 	if (wall) wall->Update();
	// }
}

void LobbyRoom::OnCharacterEnter(const std::shared_ptr<Character> &character)
{
	LOG_DEBUG("Character  {} entered the lobby", character->GetName());

	// 处理角色进入大厅的特殊逻辑
	if (character->GetType() == CharacterType::PLAYER)
	{
		// 玩家进入大厅的特殊处理
		// 例如：播放背景音乐、触发NPC对话等
	}
}

void LobbyRoom::OnCharacterExit(const std::shared_ptr<Character> &character)
{
	LOG_DEBUG("Character {} left the lobby", character->GetName());

	// 处理角色离开大厅的特殊逻辑
	if (character->GetType() == CharacterType::PLAYER)
	{
		// 玩家离开大厅的特殊处理
	}
}

void LobbyRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadLobbyObjectPosition();
	InitializeRoomObjects(jsonData);
}


void LobbyRoom::SetupWallColliders()
{
	LOG_DEBUG("Set Lobby wall collider");

	// 根据预定义的偏移和尺寸创建墙壁碰撞体
	for (size_t i = 0; i < m_WallColliderOffsets.size(); i++)
	{
		auto wallCollider = std::make_shared<nGameObject>("LobbyWall_" + std::to_string(i));

		// 設置物件的世界座標為房間中心
		wallCollider->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);

		auto collisionComponent = wallCollider->AddComponent<CollisionComponent>(ComponentType::COLLISION);

		collisionComponent->SetOffset(m_WallColliderOffsets[i]);
		collisionComponent->SetSize(m_WallColliderSizes[i]);
		collisionComponent->SetCollisionLayer(CollisionLayers_Terrain);

		LOG_DEBUG("  Created LobbyWall_{}: worldCoord({:.1f}, {:.1f}) offset({:.1f}, {:.1f}) size({:.1f}, {:.1f})", i,
				  wallCollider->GetWorldCoord().x, wallCollider->GetWorldCoord().y, m_WallColliderOffsets[i].x,
				  m_WallColliderOffsets[i].y, m_WallColliderSizes[i].x, m_WallColliderSizes[i].y);

		AddWallCollider(wallCollider);
	}
}

void LobbyRoom::AddWallCollider(const std::shared_ptr<nGameObject> &collider)
{
	if (collider)
	{
		m_WallColliders.emplace_back(collider);

		RegisterCollisionManager(collider);
		RegisterTrackingManager(collider);
	}
}

void LobbyRoom::ValidatePlayerPosition()
{
	// 簡單的防漏洞機制：只檢查玩家是否在房間區域內
	if (!IsPlayerInsideRegion() || !IsPlayerInsideRoom(16.0f))
	{
		const auto player = m_Player.lock();
		if (player)
		{
			// 將玩家移動到房間中心
			const glm::vec2 &roomCenter = m_RoomSpaceInfo.m_WorldCoord;
			player->m_WorldCoord = roomCenter;

		}
	}
}

void LobbyRoom::CreateEgg(const glm::vec2& position) {
	const auto factory = m_Factory.lock();
	if (!factory) {
		LOG_ERROR("Factory is not available");
		return;
	}

	// 使用工厂创建蛋
	auto egg = factory->CreateRoomObject("object_egg", "");
	if (!egg) {
		LOG_ERROR("Failed to create egg");
		return;
	}

	// 设置蛋的位置
	egg->SetWorldCoord(position);
	egg->SetActive(true);
	egg->SetControlVisible(true);

	// 添加到场景
	if (const auto scene = SceneManager::GetInstance().GetCurrentScene().lock()) {
		scene->GetRoot().lock()->AddChild(egg);
		scene->GetCamera().lock()->SafeAddChild(egg);
		egg->SetRegisteredToScene(true);

		// 注册到交互管理器
		if (const auto interactableManager = GetInteractionManager()) {
			interactableManager->RegisterInteractable(egg);
		}
	}

	LOG_DEBUG("Egg created at position ({}, {})", position.x, position.y);
}
