//
// Created by QuzzS on 2025/4/20.
//

#include "Room/DungeonMap.hpp"
#include <iostream>
#include <random>
#include "Creature/Character.hpp"
#include "Room/DungeonRoom.hpp"
#include "Room/MonsterRoom.hpp"
#include "Room/StartingRoom.hpp"
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

	// m_Rooms.resize(25); //

	// 十字架房間
	// for (int i=0; i < m_Rooms.size(); ++i)
	// {
	// 	const int x = i % static_cast<int>(mapSizeInGrid);
	// 	const int y = i / static_cast<int>(mapSizeInGrid);
	// 	if (x == 2 || y == 2)
	// 	{
	// 		glm::vec2 roomPosition = startPos + glm::vec2(offsetRoom, -offsetRoom) * glm::vec2(x, y);
	// 		std::shared_ptr<DungeonRoom> room;
	// 		if (x == 2 && y == 2)  room = std::make_shared<StartingRoom>(roomPosition,m_Loader.lock(),m_RoomObjectFactory.lock(),glm::vec2(x,y));
	// 		else room = std::make_shared<MonsterRoom>(roomPosition,m_Loader.lock(),m_RoomObjectFactory.lock(),glm::vec2(x,y));
	// 		m_Rooms[i] = room;
	// 		m_Rooms[i]->Start(m_Player.lock());
	// 		m_Rooms[i]->CharacterEnter(m_Player.lock());
	// 	}
	// }
	if (GenerateMainPath()) LOG_DEBUG("yes yes");

	for (int i = 0; i < std::size(m_RoomInfo); ++i) {
		if (m_RoomInfo[i].m_RoomType == RoomType::EMPTY) continue;

		const int x = i % static_cast<int>(m_SpaceInfo.roomNum.x);
		const int y = i / static_cast<int>(m_SpaceInfo.roomNum.x);
		glm::vec2 roomPosition = startPos + glm::vec2(offsetRoom, -offsetRoom) * glm::vec2(x, y);
		std::shared_ptr<DungeonRoom> room;

		switch (m_RoomInfo[i].m_RoomType) {
		case RoomType::STARTING:
			room = std::make_shared<StartingRoom>(roomPosition, m_Loader.lock(), m_RoomObjectFactory.lock(), glm::vec2(x, y));
			break;
		case RoomType::MONSTER:
			room = std::make_shared<MonsterRoom>(roomPosition, m_Loader.lock(), m_RoomObjectFactory.lock(), glm::vec2(x, y));
			break;
		case RoomType::PORTAL:
			room = std::make_shared<StartingRoom>(roomPosition, m_Loader.lock(), m_RoomObjectFactory.lock(), glm::vec2(x, y));
			break;
		default: break;
		}

		if (!room) continue;
		room->Start(m_Player.lock());
		room->CharacterEnter(m_Player.lock());
		m_RoomInfo[i].room = room;

		for (Direction dir : ALL_DIRECTIONS) {
			if (m_RoomInfo[i].m_Connections[static_cast<int>(dir)])
				room->CreateCorridorInDirection(dir);
			else
				room->CreateWallInDirection(dir);
		}
	}
	// for (int i=0; i < m_Rooms.size(); ++i)
	// {
	// 	const int x = i % static_cast<int>(mapSizeInGrid);
	// 	const int y = i / static_cast<int>(mapSizeInGrid);
	// 	if (x == 2 && y == 2)
	// 	{
	// 		glm::vec2 roomPosition = startPos + glm::vec2(offsetRoom, -offsetRoom) * glm::vec2(x, y);
	// 		const auto room = std::make_shared<DungeonRoom>(roomPosition,m_Loader.lock(),m_RoomObjectFactory.lock(),glm::vec2(x,y));
	// 		m_Rooms[i] = room;
	// 		m_Rooms[i]->Start(m_Player.lock());
	// 		m_Rooms[i]->CharacterEnter(m_Player.lock());
	// 		LOG_DEBUG("DungeonMap::Start {}",room->GetMapGridPos());
	// 	}
	// }
}

void DungeonMap::Update()
{
	// 取得當前玩家位置的房間
	UpdateCurrentRoomIfNeeded();
	// 當玩家走過門進入房間觸發轉換狀態
	if (m_CurrentRoom && m_CurrentRoom->IsPlayerInsideRoom()) m_CurrentRoom->TryActivateByPlayer();
}

bool DungeonMap::GenerateMainPath()
{
	glm::ivec2 start = {2, 2};
	int startIndex = start.y * 5 + start.x;
	m_RoomInfo[startIndex].m_RoomType = RoomType::STARTING;
	// InitRoomConnections(m_RoomInfo[startIndex]);

	auto dir1Opt = GetRandomValidDirection(start);
	if (!dir1Opt) return false;
	Direction dir1 = *dir1Opt;
	glm::ivec2 monster1 = Move(start, dir1);
	int mon1Index = monster1.y * 5 + monster1.x;
	m_RoomInfo[mon1Index].m_RoomType = RoomType::MONSTER;
	// InitRoomConnections(m_RoomInfo[mon1Index]);
	m_RoomInfo[startIndex].m_Connections[static_cast<int>(dir1)] = true;
	m_RoomInfo[mon1Index].m_Connections[static_cast<int>(GetOppositeDirection(dir1))] = true;

	auto dir2Opt = GetRandomValidDirection(monster1, {GetOppositeDirection(dir1)});
	if (!dir2Opt) return false;
	Direction dir2 = *dir2Opt;
	glm::ivec2 monster2 = Move(monster1, dir2);
	int mon2Index = monster2.y * 5 + monster2.x;
	m_RoomInfo[mon2Index].m_RoomType = RoomType::MONSTER;
	// InitRoomConnections(m_RoomInfo[mon2Index]);
	m_RoomInfo[mon1Index].m_Connections[static_cast<int>(dir2)] = true;
	m_RoomInfo[mon2Index].m_Connections[static_cast<int>(GetOppositeDirection(dir2))] = true;

	auto dir3Opt = GetRandomValidDirection(monster2, {GetOppositeDirection(dir2)});
	if (!dir3Opt) return false;
	Direction dir3 = *dir3Opt;
	glm::ivec2 portal = Move(monster2, dir3);
	int portalIndex = portal.y * 5 + portal.x;
	m_RoomInfo[portalIndex].m_RoomType = RoomType::PORTAL;
	// InitRoomConnections(m_RoomInfo[portalIndex]);
	m_RoomInfo[mon2Index].m_Connections[static_cast<int>(dir3)] = true;
	m_RoomInfo[portalIndex].m_Connections[static_cast<int>(GetOppositeDirection(dir3))] = true;

	LOG_DEBUG("Main path: START ({}, {}) -> MON1 ({}, {}) -> MON2 ({}, {}) -> PORTAL ({}, {})",
			  start.x, start.y, monster1.x, monster1.y, monster2.x, monster2.y, portal.x, portal.y);
	return true;
}


void DungeonMap::UpdateCurrentRoomIfNeeded()
{
	// 找物体所在的房间
	if (!m_CurrentRoom || !m_CurrentRoom->IsPlayerInsideRegion())
	{
		const auto mapRoomCount = m_SpaceInfo.roomNum;
		const auto roomSizeInPixel = m_SpaceInfo.tileSize * m_SpaceInfo.roomRegion;
		const glm::ivec2 IndexInside = Tool::FindIndexWhichGridObjectIsInside(
			mapRoomCount,
			roomSizeInPixel,
			m_Player.lock()->GetWorldCoord());
		if (IndexInside.x <0 || IndexInside.y < 0)
		{
			LOG_ERROR("DungeonMap::UpdateCurrentRoomIfNeeded error IndexInside");
			return;
		}
		m_CurrentRoom = m_RoomInfo[IndexInside.y * 5 + IndexInside.x].room;
		// if(m_CurrentRoom) m_CurrentRoom->OnStateChanged();
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
			if (!m_RoomInfo[roomIndex].room) continue;; // TODO:目前是九宫格，但可能不需要
			neighborRooms[row * nearRegionSize + col] = m_RoomInfo[roomIndex].room;
		}
		std::cout << std::endl;
	}
	return neighborRooms;
}

Direction DungeonMap::GetOppositeDirection(Direction dir) {
	switch (dir) {
	case Direction::UP: return Direction::DOWN;
	case Direction::DOWN: return Direction::UP;
	case Direction::LEFT: return Direction::RIGHT;
	case Direction::RIGHT: return Direction::LEFT;
	}
	return Direction::UP;
}

glm::ivec2 DungeonMap::Move(const glm::ivec2& pos, Direction dir) {
	switch (dir) {
	case Direction::UP: return pos + glm::ivec2(0, -1);
	case Direction::DOWN: return pos + glm::ivec2(0, 1);
	case Direction::LEFT: return pos + glm::ivec2(-1, 0);
	case Direction::RIGHT: return pos + glm::ivec2(1, 0);
	}
	return pos;
}

std::optional<Direction> DungeonMap::GetRandomValidDirection(const glm::ivec2 &currentPos,
															 const std::set<Direction> &exclude)
{
	std::vector<Direction> dirs = {Direction::UP, Direction::RIGHT, Direction::DOWN, Direction::LEFT};
	std::shuffle(dirs.begin(), dirs.end(), std::mt19937(std::random_device{}()));

	for (Direction dir : dirs) {
		if (exclude.count(dir)) continue;
		glm::ivec2 next = Move(currentPos, dir);

		// 檢查邊界
		if (next.x < 0 || next.x >= 5 || next.y < 0 || next.y >= 5)
			continue;

		int index = next.y * 5 + next.x;
		if (m_RoomInfo[index].m_RoomType != RoomType::EMPTY)
			continue;

		return dir;
	}

	return std::nullopt;
}




