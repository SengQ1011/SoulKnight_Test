//
// Created by QuzzS on 2025/4/25.
//

#include "Room/SpecialRoom.hpp"
#include "Components/InteractableComponent.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "RandomUtil.hpp"
#include "RoomObject/DestructibleObject.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Logger.hpp"

void SpecialRoom::Start(const std::shared_ptr<Character> &player)
{
	DungeonRoom::Start(player);
	SpawnSpecialObjects();
}

void SpecialRoom::Update() { DungeonRoom::Update(); }

void SpecialRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadPortalRoomObjectPosition();
	InitializeRoomObjects(jsonData);
}

void SpecialRoom::SpawnSpecialObjects()
{
	// 隨機選擇生成類型：礦物或武器寶箱
	const float randomValue = RandomUtil::RandomFloatInRange(0.0f, 1.0f);

	if (randomValue < 0.3f) // 40% 機率生成金礦
	{
		const auto specialObject = m_Factory.lock()->CreateMine(MineType::GOLD);
		if (!specialObject)
		{
			LOG_ERROR("Failed to create Gold Mine");
			return;
		}

		// 設置位置在房間中央
		specialObject->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);
		AddRoomObject(specialObject);

		LOG_DEBUG("SpecialRoom: Spawned Gold Mine at center");
	}
	else if (randomValue < 0.6f) // 30% 機率生成能量礦
	{
		const auto specialObject = m_Factory.lock()->CreateMine(MineType::ENERGY);
		if (!specialObject)
		{
			LOG_ERROR("Failed to create Energy Mine");
			return;
		}

		// 設置位置在房間中央
		specialObject->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);
		AddRoomObject(specialObject);

		LOG_DEBUG("SpecialRoom: Spawned Energy Mine at center");
	}
	else // 30% 機率生成武器寶箱
	{
		const auto weaponChest = CreateChest(ChestType::WEAPON);
		if (!weaponChest)
		{
			LOG_ERROR("Failed to create Weapon Chest");
			return;
		}

		// 設置位置在房間中央
		weaponChest->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);
		AddRoomObject(weaponChest);

		LOG_DEBUG("SpecialRoom: Spawned Weapon Chest at center");
	}
}
