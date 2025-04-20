//
// Created by QuzzS on 2025/4/20.
//

#include "Room/DungeonMap.hpp"
#include <iostream>
#include "Creature/Character.hpp"
#include "Room/DungeonRoom.hpp"
#include "Tool/Tool.hpp"
#include "Util/Input.hpp"

void DungeonMap::Start()
{
	const float tileSize = m_SpaceInfo.tileSize.x;
	const float roomRegion = m_SpaceInfo.roomRegion.x;
	const float mapSizeInGrid = m_SpaceInfo.roomNum.x;
	auto startPos = glm::vec2(std::floor(mapSizeInGrid/2) * roomRegion * tileSize);
	startPos *= glm::vec2(-1,1); // 左上0，0房间坐标
	const float offsetRoom = tileSize * roomRegion;

	m_Rooms.resize(25); //

	for (int i=0; i < m_Rooms.size(); ++i)
	{
		const int x = i % static_cast<int>(mapSizeInGrid);
		const int y = i / static_cast<int>(mapSizeInGrid);
		if (x == 0 || x == 4 || y == 0 || y == 4) continue;
		glm::vec2 roomPosition = startPos + glm::vec2(offsetRoom, -offsetRoom) * glm::vec2(x, y);
		const auto room = std::make_shared<DungeonRoom>(roomPosition,m_Loader.lock(),m_RoomObjectFactory.lock(),glm::vec2(x,y));
		m_Rooms[i] = room;
		m_Rooms[i]->Start(m_Player.lock());
		m_Rooms[i]->CharacterEnter(m_Player.lock());
		LOG_DEBUG("DungeonMap::Start {}",room->GetMapGridPos());
	}
}

void DungeonMap::Update()
{
	// for(int i=0; i<m_Rooms.size(); i++)
	// {
	// 	if (!m_Rooms[i]->IsPlayerInside()) continue;
	// 	m_CurrentRoom = m_Rooms[i];
	// 	if (Util::Input::IsKeyPressed(Util::Keycode::NUM_1)) LOG_DEBUG("DungeonRoom {} {}", i%5, i/5);
	// }
	UpdateCurrentRoomIfNeeded();

	// 测试成功
	if (Util::Input::IsKeyUp(Util::Keycode::NUM_2))
	{
		const std::vector<std::weak_ptr<DungeonRoom>>& rooms = GetNeighborRooms();
		LOG_DEBUG("DungeonRoom {}",rooms.size());
		for (int i=0; i<rooms.size(); i++)
		{
			const auto room = rooms[i].lock();
			if (room)
			{
				std::cout << room->GetMapGridPos().x << "," << room->GetMapGridPos().y << " ";
			}
			else
			{
				std::cout << "null ";
			}
			if ((i + 1) % 3 == 0) std::cout << std::endl;
		}

	}
}

void DungeonMap::UpdateCurrentRoomIfNeeded()
{
	// 找物体所在的房间
	if (!m_CurrentRoom || !m_CurrentRoom->IsPlayerInside())
	{
		const auto mapRoomCount = m_SpaceInfo.roomNum;
		const auto roomSizeInPixel = m_SpaceInfo.tileSize * m_SpaceInfo.roomRegion;
		const glm::ivec2 IndexInside = Tool::FindIndexWhichGridObjectIsInside(
			mapRoomCount,
			roomSizeInPixel,
			m_Player.lock()->GetWorldCoord());
		m_CurrentRoom = m_Rooms[IndexInside.y * 5 + IndexInside.x];
	}
}

// 用途只有检查上下左右吗？
std::vector<std::weak_ptr<DungeonRoom>> DungeonMap::GetNeighborRooms() const
{
	const glm::vec2 currentRoomWorldPos = m_CurrentRoom->GetRoomSpaceInfo().m_WorldCoord;

	// 左上角為 (0,0) 的原點世界座標
	const glm::vec2 roomSize = m_SpaceInfo.roomRegion * m_SpaceInfo.tileSize;
	const glm::vec2 halfMapRoomCount = glm::floor(m_SpaceInfo.roomNum / 2.0f);
	const glm::vec2 startWorldPos = roomSize * halfMapRoomCount * glm::vec2(-1, 1);

	// 換算為格子座標（相對於地圖左上）
	glm::vec2 currentRoomGridPos = (currentRoomWorldPos - startWorldPos) / roomSize;
	currentRoomGridPos *= glm::vec2(1,-1);
	std::vector<std::weak_ptr<DungeonRoom>> neighborRooms;
	constexpr int nearRegionSize = 3;
	neighborRooms.resize(nearRegionSize * nearRegionSize);

	for (int row = 0; row < nearRegionSize; row++)
	{
		const int RoomGridPosRow = static_cast<int>(currentRoomGridPos.y) + row - 1;
		// Map纵轴边界判断
		if (RoomGridPosRow < 0 ||
			RoomGridPosRow >= static_cast<int>(m_SpaceInfo.roomNum.y)) continue;
		for (int col = 0; col < nearRegionSize; col++)
		{
			const int RoomGridPosCol = static_cast<int>(currentRoomGridPos.x) + col - 1;
			// Map横轴边界判断
			if (RoomGridPosCol < 0 ||
				RoomGridPosCol >= static_cast<int>(m_SpaceInfo.roomNum.x)) continue;
			const int roomIndex = RoomGridPosRow * static_cast<int>(m_SpaceInfo.roomNum.x) + RoomGridPosCol;
			if (!m_Rooms[roomIndex]) continue; // TODO:目前是九宫格，但可能不需要
			neighborRooms[row * nearRegionSize + col] = m_Rooms[RoomGridPosRow * static_cast<int>(m_SpaceInfo.roomNum.x) + RoomGridPosCol];
		}
		std::cout << std::endl;
	}
	return neighborRooms;
}


