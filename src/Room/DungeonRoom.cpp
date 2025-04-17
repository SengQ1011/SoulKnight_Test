//
// Created by QuzzS on 2025/4/12.
//

#include "Room/DungeonRoom.hpp"

void DungeonRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadStartingRoomObjectPosition();
	InitializeRoomObjects(jsonData);
}

bool DungeonRoom::IsPlayerInside() const
{
	const auto player = m_Player.lock();
	if (!player) return false;
	return (player->m_WorldCoord.x < m_WorldCoord.x + m_RoomRegion.x/2.0f) &&
	   (player->m_WorldCoord.y < m_WorldCoord.y + m_RoomRegion.y/2.0f) &&
	   (player->m_WorldCoord.x > m_WorldCoord.x - m_RoomRegion.x/2.0f) &&
	   (player->m_WorldCoord.y > m_WorldCoord.y - m_RoomRegion.y/2.0f);
}


