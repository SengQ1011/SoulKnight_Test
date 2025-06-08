//
// Created by QuzzS on 2025/4/25.
//

#include "Room/SpecialRoom.hpp"
#include "Components/InteractableComponent.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
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
	// 創建特殊物件 - 暫時使用紅色箱子作為示例
	const auto specialObject = m_Factory.lock()->CreateRoomObject("object_boxRed", "DestructibleObject");
	if (!specialObject)
	{
		LOG_ERROR("Failed to create special object");
		return;
	}

	// 設置位置在房間中央
	specialObject->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);

	AddRoomObject(specialObject);

	LOG_DEBUG("SpecialRoom: Spawned special objects");
}
