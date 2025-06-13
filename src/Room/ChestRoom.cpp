//
// Created by QuzzS on 2025/4/25.
//

#include "Room/ChestRoom.hpp"
#include "Components/InteractableComponent.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "Util/Logger.hpp"

void ChestRoom::Start(const std::shared_ptr<Character> &player)
{
	DungeonRoom::Start(player);

	// 創建寶箱
	SpawnChest();

	LOG_DEBUG("ChestRoom started at grid position ({}, {})", m_MapGridPos.x, m_MapGridPos.y);
}

void ChestRoom::Update() { DungeonRoom::Update(); }

void ChestRoom::LoadFromJSON()
{
	// 暫時使用空的JSON數據，未來可以添加特定的寶箱房間佈局
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadPortalRoomObjectPosition();
	;
	InitializeRoomObjects(jsonData);
}

void ChestRoom::TryActivateByPlayer()
{
	// 寶箱房間被玩家進入時立即設為已探索
	SetState(RoomState::EXPLORED);
	LOG_DEBUG("ChestRoom activated by player");
}

void ChestRoom::OnStateChanged()
{
	// 當房間狀態改變時的處理
	switch (GetState())
	{
	case RoomState::EXPLORED:
		LOG_DEBUG("ChestRoom explored - chest available for interaction");
		break;
	case RoomState::CLEANED:
		LOG_DEBUG("ChestRoom cleaned - chest has been opened");
		break;
	default:
		break;
	}
}

void ChestRoom::SpawnChest()
{
	if (const auto rewardChest = CreateChest(ChestType::WEAPON))
	{
		rewardChest->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);
	}
}

void ChestRoom::OnChestOpened()
{
	if (m_ChestOpened)
	{
		LOG_DEBUG("Chest already opened");
		return;
	}

	m_ChestOpened = true;
	SetState(RoomState::CLEANED);

	// TODO: 在這裡添加寶箱開啟的邏輯
	// - 生成隨機獎勵（武器、金幣、道具等）
	// - 播放開箱動畫
	// - 播放音效

	LOG_INFO("Chest opened! Rewards granted to player.");
}
