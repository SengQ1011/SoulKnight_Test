//
// Created by QuzzS on 2025/4/24.
//

#include "Room/StartingRoom.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"

void StartingRoom::Start(const std::shared_ptr<Character> &player) { DungeonRoom::Start(player); }


void StartingRoom::Update() { DungeonRoom::Update(); }

void StartingRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadStartingRoomObjectPosition();
	InitializeRoomObjects(jsonData);
}
