//
// Created by QuzzS on 2025/4/20.
//

#include "Room/DungeonMap.hpp"
#include <iostream>
#include <random>
#include "Creature/Character.hpp"
#include "Room/BossRoom.hpp"
#include "Room/ChestRoom.hpp"
#include "Room/DungeonRoom.hpp"
#include "Room/MonsterRoom.hpp"
#include "Room/PortalRoom.hpp"
#include "Room/ShopRoom.hpp"
#include "Room/SpecialRoom.hpp"
#include "Room/StartingRoom.hpp"
#include "SaveManager.hpp"
#include "Tool/Tool.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"

void DungeonMap::Start()
{
	const float tileSize = m_SpaceInfo.tileSize.x;
	const float roomRegion = m_SpaceInfo.roomRegion.x;
	const float mapSizeInGrid = m_SpaceInfo.roomNum.x;
	auto startPos = glm::vec2(std::floor(mapSizeInGrid / 2) * roomRegion * tileSize);
	startPos *= glm::vec2(-1, 1); // 左上0，0房间坐标
	const float offsetRoom = tileSize * roomRegion;

	// 产生了主要路径
	GenerateMainPath();

	// 生成分支房間
	GenerateBranches();

	for (int i = 0; i < std::size(m_RoomInfo); ++i)
	{
		if (m_RoomInfo[i].m_RoomType == RoomType::EMPTY)
			continue;

		const int x = i % static_cast<int>(m_SpaceInfo.roomNum.x);
		const int y = i / static_cast<int>(m_SpaceInfo.roomNum.x);
		glm::vec2 roomPosition = startPos + glm::vec2(offsetRoom, -offsetRoom) * glm::vec2(x, y);
		std::shared_ptr<DungeonRoom> room;

		switch (m_RoomInfo[i].m_RoomType)
		{
		case RoomType::STARTING:
			room = std::make_shared<StartingRoom>(roomPosition, m_Loader.lock(), m_RoomObjectFactory.lock(),
												  glm::vec2(x, y));
			break;
		case RoomType::MONSTER:
			room = std::make_shared<MonsterRoom>(roomPosition, m_Loader.lock(), m_RoomObjectFactory.lock(),
												 glm::vec2(x, y));
			break;
		case RoomType::BOSS:
			room =
				std::make_shared<BossRoom>(roomPosition, m_Loader.lock(), m_RoomObjectFactory.lock(), glm::vec2(x, y));
			break;
		case RoomType::PORTAL:
			room = std::make_shared<PortalRoom>(roomPosition, m_Loader.lock(), m_RoomObjectFactory.lock(),
												glm::vec2(x, y));
			break;
		case RoomType::CHEST:
			room =
				std::make_shared<ShopRoom>(roomPosition, m_Loader.lock(), m_RoomObjectFactory.lock(), glm::vec2(x, y));
			break;
		case RoomType::SPECIAL:
			room = std::make_shared<SpecialRoom>(roomPosition, m_Loader.lock(), m_RoomObjectFactory.lock(),
												 glm::vec2(x, y));
			break;
		default:
			break;
		}

		if (!room)
			continue;
		room->Start(m_Player.lock());
		// 注意：不在初始化時就將玩家添加到所有房間
		// room->PlayerEnter(m_Player.lock());
		m_RoomInfo[i].room = room;

		for (Direction dir : ALL_DIRECTIONS)
		{
			if (m_RoomInfo[i].m_Connections[static_cast<int>(dir)])
				room->CreateCorridorInDirection(dir);
			else
				room->CreateWallInDirection(dir);
		}

		// 在所有地形生成完成後進行最終設置和優化
		room->FinalizeRoomSetup();
	}

	// 設置房間間的連接關係
	SetupRoomConnections();
}

void DungeonMap::Update()
{
	// 取得當前玩家位置的房間
	UpdateCurrentRoomIfNeeded();
	// 當玩家走過門進入房間觸發轉換狀態
	if (m_CurrentRoom && m_CurrentRoom->IsPlayerInsideRoom())
		m_CurrentRoom->TryActivateByPlayer();

	// 合法玩家位置 -- 怕出bug
	if (!m_CurrentRoom->IsPlayerInValidPosition())
	{
		const auto player = m_Player.lock();
		if (!player)
			return;

		const auto prevRoom = m_PreviousRoom.lock();
		if (prevRoom)
		{
			player->SetWorldCoord(prevRoom->GetRoomSpaceInfo().m_WorldCoord);
		}
		else
		{
			player->SetWorldCoord(m_CurrentRoom->GetRoomSpaceInfo().m_WorldCoord);
		}
	}
}

bool DungeonMap::GenerateMainPath()
{
	glm::ivec2 start = {2, 2};
	int startIndex = start.y * 5 + start.x;
	m_RoomInfo[startIndex].m_RoomType = RoomType::STARTING;

	auto dir1Opt = GetRandomValidDirection(start);
	if (!dir1Opt)
		return false;
	Direction dir1 = *dir1Opt;
	glm::ivec2 monster1 = Move(start, dir1);
	int mon1Index = monster1.y * 5 + monster1.x;
	m_RoomInfo[mon1Index].m_RoomType = RoomType::MONSTER;
	m_RoomInfo[startIndex].m_Connections[static_cast<int>(dir1)] = true;
	m_RoomInfo[mon1Index].m_Connections[static_cast<int>(GetOppositeDirection(dir1))] = true;

	auto dir2Opt = GetRandomValidDirection(monster1, {GetOppositeDirection(dir1)});
	if (!dir2Opt)
		return false;
	Direction dir2 = *dir2Opt;
	glm::ivec2 monster2 = Move(monster1, dir2);
	int mon2Index = monster2.y * 5 + monster2.x;
	m_RoomInfo[mon2Index].m_RoomType = RoomType::MONSTER;
	m_RoomInfo[mon1Index].m_Connections[static_cast<int>(dir2)] = true;
	m_RoomInfo[mon2Index].m_Connections[static_cast<int>(GetOppositeDirection(dir2))] = true;

	auto dir3Opt = GetRandomValidDirection(monster2, {GetOppositeDirection(dir2)});
	if (!dir3Opt)
		return false;
	Direction dir3 = *dir3Opt;
	glm::ivec2 finalRoom = Move(monster2, dir3);
	int finalRoomIndex = finalRoom.y * 5 + finalRoom.x;

	// 檢查當前關卡是否為第5關（最後一關）
	bool isLastStage = false;
	if (SaveManager::GetInstance().HasSaveData())
	{
		auto saveData = SaveManager::GetInstance().GetSaveData();
		if (saveData && saveData->gameProgress.currentStage == 5)
		{
			isLastStage = true;
		}
	}

	// 根據是否為最後一關決定房間類型
	if (isLastStage)
	{
		m_RoomInfo[finalRoomIndex].m_RoomType = RoomType::BOSS;
	}
	else
	{
		m_RoomInfo[finalRoomIndex].m_RoomType = RoomType::PORTAL;
	}

	m_RoomInfo[mon2Index].m_Connections[static_cast<int>(dir3)] = true;
	m_RoomInfo[finalRoomIndex].m_Connections[static_cast<int>(GetOppositeDirection(dir3))] = true;
	return true;
}

bool DungeonMap::GenerateBranches()
{
	// 收集主路徑上的 MONSTER 房間位置
	std::vector<glm::ivec2> monsterPositions;
	for (int i = 0; i < 25; ++i)
	{
		if (m_RoomInfo[i].m_RoomType == RoomType::MONSTER)
		{
			const int x = i % 5;
			const int y = i / 5;
			monsterPositions.push_back({x, y});
		}
	}

	if (monsterPositions.size() != 2)
	{
		LOG_ERROR("GenerateBranches: Expected 2 MONSTER rooms, found {}", monsterPositions.size());
		return false;
	}

	// 收集所有主路徑 MONSTER 房間的可用方向
	std::vector<std::pair<glm::ivec2, Direction>> availableDirections;
	for (const auto &monsterPos : monsterPositions)
	{
		int monsterIndex = monsterPos.y * 5 + monsterPos.x;

		// 檢查四個方向，排除已經被主路徑佔用的方向
		for (Direction dir : ALL_DIRECTIONS)
		{
			// 跳過已經有連接的方向（主路徑方向）
			if (m_RoomInfo[monsterIndex].m_Connections[static_cast<int>(dir)])
				continue;

			glm::ivec2 nextPos = Move(monsterPos, dir);

			// 邊界檢查
			if (nextPos.x < 0 || nextPos.x >= 5 || nextPos.y < 0 || nextPos.y >= 5)
				continue;

			int nextIndex = nextPos.y * 5 + nextPos.x;
			// 確保目標位置是空的
			if (m_RoomInfo[nextIndex].m_RoomType != RoomType::EMPTY)
				continue;

			availableDirections.push_back({monsterPos, dir});
		}
	}

	if (availableDirections.empty())
	{
		LOG_WARN("GenerateBranches: No available directions for branch generation");
		return true; // 不算失敗，只是沒有空間生成分支
	}

	// Step 1: 確保生成至少 1個 CHEST 和 1個 SPECIAL
	bool chestGenerated = false;
	bool specialGenerated = false;
	size_t directionsUsed = 0;

	// 隨機排列可用方向
	std::shuffle(availableDirections.begin(), availableDirections.end(), std::mt19937(std::random_device{}()));

	// 優先生成 CHEST
	if (directionsUsed < availableDirections.size() && !chestGenerated)
	{
		auto [monsterPos, dir] = availableDirections[directionsUsed];
		glm::ivec2 branchPos = Move(monsterPos, dir);

		int monsterIndex = monsterPos.y * 5 + monsterPos.x;
		int branchIndex = branchPos.y * 5 + branchPos.x;

		m_RoomInfo[branchIndex].m_RoomType = RoomType::CHEST;
		m_RoomInfo[monsterIndex].m_Connections[static_cast<int>(dir)] = true;
		m_RoomInfo[branchIndex].m_Connections[static_cast<int>(GetOppositeDirection(dir))] = true;

		chestGenerated = true;
		directionsUsed++;

		// LOG_DEBUG("Generated guaranteed CHEST at ({}, {}) from MONSTER at ({}, {})", branchPos.x, branchPos.y,
				  // monsterPos.x, monsterPos.y);
	}

	// 然後生成 SPECIAL
	if (directionsUsed < availableDirections.size() && !specialGenerated)
	{
		auto [monsterPos, dir] = availableDirections[directionsUsed];
		glm::ivec2 branchPos = Move(monsterPos, dir);

		int monsterIndex = monsterPos.y * 5 + monsterPos.x;
		int branchIndex = branchPos.y * 5 + branchPos.x;

		m_RoomInfo[branchIndex].m_RoomType = RoomType::SPECIAL;
		m_RoomInfo[monsterIndex].m_Connections[static_cast<int>(dir)] = true;
		m_RoomInfo[branchIndex].m_Connections[static_cast<int>(GetOppositeDirection(dir))] = true;

		specialGenerated = true;
		directionsUsed++;
	}

	// Step 2: 剩餘方向隨機生成房間 (MONSTER: 60%, SPECIAL: 35%, CHEST: 5%)
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> roomGenDist(0.0, 1.0);
	std::uniform_real_distribution<> roomTypeDist(0.0, 1.0);

	for (size_t i = directionsUsed; i < availableDirections.size(); ++i)
	{
		// 50% 機率生成房間
		if (roomGenDist(gen) > 0.5)
			continue;

		auto [monsterPos, dir] = availableDirections[i];
		glm::ivec2 branchPos = Move(monsterPos, dir);

		int monsterIndex = monsterPos.y * 5 + monsterPos.x;
		int branchIndex = branchPos.y * 5 + branchPos.x;

		// 根據機率選擇房間類型
		double typeRoll = roomTypeDist(gen);
		RoomType branchType;
		if (typeRoll < 0.6) // 60%
		{
			branchType = RoomType::MONSTER;
		}
		else if (typeRoll < 0.95) // 35%
		{
			branchType = RoomType::SPECIAL;
		}
		else // 5%
		{
			branchType = RoomType::CHEST;
		}

		m_RoomInfo[branchIndex].m_RoomType = branchType;
		m_RoomInfo[monsterIndex].m_Connections[static_cast<int>(dir)] = true;
		m_RoomInfo[branchIndex].m_Connections[static_cast<int>(GetOppositeDirection(dir))] = true;
	}

	// Step 3: 建立相鄰分支房間的連接（排除 STARTING/PORTAL）
	GenerateAdjacentConnections();

	return true;
}

void DungeonMap::GenerateAdjacentConnections()
{
	for (int i = 0; i < 25; ++i)
	{
		// 跳過空房間、STARTING、PORTAL 和 BOSS 房間
		if (m_RoomInfo[i].m_RoomType == RoomType::EMPTY || m_RoomInfo[i].m_RoomType == RoomType::STARTING ||
			m_RoomInfo[i].m_RoomType == RoomType::PORTAL || m_RoomInfo[i].m_RoomType == RoomType::BOSS)
			continue;

		const int x = i % 5;
		const int y = i / 5;

		// 檢查四個方向的相鄰房間
		for (Direction dir : ALL_DIRECTIONS)
		{
			glm::ivec2 neighborPos = glm::ivec2(x, y);
			switch (dir)
			{
			case Direction::UP:
				neighborPos.y -= 1;
				break;
			case Direction::DOWN:
				neighborPos.y += 1;
				break;
			case Direction::LEFT:
				neighborPos.x -= 1;
				break;
			case Direction::RIGHT:
				neighborPos.x += 1;
				break;
			}

			// 邊界檢查
			if (neighborPos.x < 0 || neighborPos.x >= 5 || neighborPos.y < 0 || neighborPos.y >= 5)
				continue;

			int neighborIndex = neighborPos.y * 5 + neighborPos.x;

			// 如果相鄰房間存在且不是 STARTING/PORTAL/BOSS，建立連接
			if (m_RoomInfo[neighborIndex].m_RoomType != RoomType::EMPTY &&
				m_RoomInfo[neighborIndex].m_RoomType != RoomType::STARTING &&
				m_RoomInfo[neighborIndex].m_RoomType != RoomType::PORTAL &&
				m_RoomInfo[neighborIndex].m_RoomType != RoomType::BOSS)
			{
				// 檢查是否已經有連接，避免重複設置
				if (!m_RoomInfo[i].m_Connections[static_cast<int>(dir)])
				{
					m_RoomInfo[i].m_Connections[static_cast<int>(dir)] = true;
					m_RoomInfo[neighborIndex].m_Connections[static_cast<int>(GetOppositeDirection(dir))] = true;

					LOG_DEBUG("Created adjacent connection between ({}, {}) and ({}, {})", x, y, neighborPos.x,
							  neighborPos.y);
				}
			}
		}
	}
}

void DungeonMap::UpdateCurrentRoomIfNeeded()
{
	// 找物体所在的房间
	if (!m_CurrentRoom || !m_CurrentRoom->IsPlayerInsideRegion())
	{
		const auto mapRoomCount = m_SpaceInfo.roomNum;
		const auto roomSizeInPixel = m_SpaceInfo.tileSize * m_SpaceInfo.roomRegion;
		const glm::ivec2 IndexInside =
			Tool::FindIndexWhichGridObjectIsInside(mapRoomCount, roomSizeInPixel, m_Player.lock()->GetWorldCoord());
		if (IndexInside.x < 0 || IndexInside.y < 0)
		{
			LOG_ERROR("DungeonMap::UpdateCurrentRoomIfNeeded error IndexInside");
			return;
		}

		// 從舊房間移除玩家
		if (m_CurrentRoom)
		{
			m_CurrentRoom->RemovePlayerFromList(m_Player.lock());
			m_PreviousRoom = m_CurrentRoom; // 记录前一个房间 -- 重置位置用
		}

		// 切換到新房間並添加玩家
		m_CurrentRoom = m_RoomInfo[IndexInside.y * 5 + IndexInside.x].room;
		if (m_CurrentRoom)
		{
			m_CurrentRoom->PlayerEnter(m_Player.lock());
		}
	}
}

// 用途只有检查上下左右吗？
std::vector<std::weak_ptr<DungeonRoom>> DungeonMap::GetNeighborRooms() const
{
	if (!m_CurrentRoom)
	{
		return {};
	}

	// 直接使用當前房間的連接關係
	return m_CurrentRoom->GetConnectedNeighbors();
}

Direction DungeonMap::GetOppositeDirection(Direction dir)
{
	switch (dir)
	{
	case Direction::UP:
		return Direction::DOWN;
	case Direction::DOWN:
		return Direction::UP;
	case Direction::LEFT:
		return Direction::RIGHT;
	case Direction::RIGHT:
		return Direction::LEFT;
	}
	return Direction::UP;
}

glm::ivec2 DungeonMap::Move(const glm::ivec2 &pos, Direction dir)
{
	switch (dir)
	{
	case Direction::UP:
		return pos + glm::ivec2(0, -1);
	case Direction::DOWN:
		return pos + glm::ivec2(0, 1);
	case Direction::LEFT:
		return pos + glm::ivec2(-1, 0);
	case Direction::RIGHT:
		return pos + glm::ivec2(1, 0);
	}
	return pos;
}

std::optional<Direction> DungeonMap::GetRandomValidDirection(const glm::ivec2 &currentPos,
															 const std::set<Direction> &exclude)
{
	std::vector dirs = {Direction::UP, Direction::RIGHT, Direction::DOWN, Direction::LEFT};
	std::shuffle(dirs.begin(), dirs.end(), std::mt19937(std::random_device{}()));

	for (Direction dir : dirs)
	{
		if (exclude.count(dir))
			continue;
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

void DungeonMap::SetupRoomConnections()
{
	for (int i = 0; i < std::size(m_RoomInfo); ++i)
	{
		if (m_RoomInfo[i].m_RoomType == RoomType::EMPTY || !m_RoomInfo[i].room)
			continue;

		const int x = i % static_cast<int>(m_SpaceInfo.roomNum.x);
		const int y = i / static_cast<int>(m_SpaceInfo.roomNum.x);

		// 檢查四個方向的相鄰房間
		for (Direction dir : ALL_DIRECTIONS)
		{
			glm::ivec2 neighborPos = glm::ivec2(x, y);
			switch (dir)
			{
			case Direction::UP:
				neighborPos.y -= 1;
				break;
			case Direction::DOWN:
				neighborPos.y += 1;
				break;
			case Direction::LEFT:
				neighborPos.x -= 1;
				break;
			case Direction::RIGHT:
				neighborPos.x += 1;
				break;
			}

			// 邊界檢查
			if (neighborPos.x < 0 || neighborPos.x >= 5 || neighborPos.y < 0 || neighborPos.y >= 5)
				continue;

			int neighborIndex = neighborPos.y * 5 + neighborPos.x;

			// 如果相鄰位置有房間，設置連接關係
			if (m_RoomInfo[neighborIndex].room)
			{
				bool hasConnection = m_RoomInfo[i].m_Connections[static_cast<int>(dir)];
				m_RoomInfo[i].room->SetNeighborRoom(dir, m_RoomInfo[neighborIndex].room, hasConnection);
			}
		}
	}
}
