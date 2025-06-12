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
	// 隨機選擇礦物類型
	const float randomValue = RandomUtil::RandomFloatInRange(0.0f, 1.0f);
	MineType mineType;
	std::string mineTypeName;

	if (randomValue < 0.5f) // 50% 機率
	{
		mineType = MineType::GOLD;
		mineTypeName = "Gold Mine";
	}
	else
	{
		mineType = MineType::ENERGY;
		mineTypeName = "Energy Mine";
	}

	// 創建隨機選擇的礦物
	const auto specialObject = m_Factory.lock()->CreateMine(mineType);
	if (!specialObject)
	{
		LOG_ERROR("Failed to create special object: {}", mineTypeName);
		return;
	}

	// 設置位置在房間中央
	specialObject->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);

	AddRoomObject(specialObject);

	LOG_DEBUG("SpecialRoom: Spawned {} at center", mineTypeName);
}
