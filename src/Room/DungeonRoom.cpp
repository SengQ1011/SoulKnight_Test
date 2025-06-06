//
// Created by QuzzS on 2025/4/12.
//

#include "Room/DungeonRoom.hpp"

#include <algorithm>
#include <iostream>


#include "Camera.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/DoorComponent.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"

#include "Creature/Character.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Loader.hpp"
#include "RoomObject/WallObject.hpp"
#include "Tool/Tool.hpp"


// ===== RoomConnectionManager 實現 =====
void RoomConnectionManager::SetNeighbor(Direction dir, const std::weak_ptr<DungeonRoom> &room)
{
	m_Neighbors[static_cast<int>(dir)] = room;
}

void RoomConnectionManager::SetConnection(Direction dir, bool connected)
{
	m_HasConnection[static_cast<int>(dir)] = connected;
}

std::weak_ptr<DungeonRoom> RoomConnectionManager::GetNeighbor(Direction dir) const
{
	return m_Neighbors[static_cast<int>(dir)];
}

bool RoomConnectionManager::HasConnection(Direction dir) const { return m_HasConnection[static_cast<int>(dir)]; }

std::vector<std::weak_ptr<DungeonRoom>> RoomConnectionManager::GetConnectedNeighbors() const
{
	std::vector<std::weak_ptr<DungeonRoom>> connected;
	for (int i = 0; i < 4; ++i)
	{
		if (m_HasConnection[i] && !m_Neighbors[i].expired())
		{
			connected.push_back(m_Neighbors[i]);
		}
	}
	return connected;
}

std::vector<Direction> RoomConnectionManager::GetConnectedDirections() const
{
	std::vector<Direction> directions;
	for (int i = 0; i < 4; ++i)
	{
		if (m_HasConnection[i])
		{
			directions.push_back(static_cast<Direction>(i));
		}
	}
	return directions;
}

// ===== GridSystem 實現 =====
GridSystem::GridSystem() : m_Grid(RoomConstants::GRID_SIZE, std::vector<int>(RoomConstants::GRID_SIZE, 0)) {}

void GridSystem::Initialize()
{
	// 重置網格
	for (auto &row : m_Grid)
	{
		std::fill(row.begin(), row.end(), 0);
	}
}

void GridSystem::MarkPosition(int row, int col, int value)
{
	if (IsValidPosition(row, col))
	{
		m_Grid[row][col] = value;
	}
}

bool GridSystem::IsPositionBlocked(int row, int col) const
{
	if (!IsValidPosition(row, col))
		return true;
	return m_Grid[row][col] != 0;
}

bool GridSystem::IsValidPosition(int row, int col) const
{
	return row >= 0 && row < RoomConstants::GRID_SIZE && col >= 0 && col < RoomConstants::GRID_SIZE;
}

void GridSystem::UpdateGridFromObjects(const std::vector<std::shared_ptr<nGameObject>> &objects,
									   const RoomSpaceInfo &spaceInfo)
{
	const float gridSize = spaceInfo.m_RoomRegion.x;
	const float tileSize = spaceInfo.m_TileSize.x;

	// 計算左上角格子世界坐標軸
	auto startPos = glm::vec2(gridSize * tileSize / 2) - glm::vec2(tileSize / 2);
	startPos *= glm::vec2(-1, 1);
	startPos += spaceInfo.m_WorldCoord;

	for (const auto &elem : objects)
	{
		auto collisionComp = elem->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		if (!collisionComp)
			continue;

		Rect bound = collisionComp->GetBounds();
		glm::ivec2 startGrid = glm::vec2(std::floor((bound.left() - startPos.x) / tileSize),
										 std::ceil((startPos.y - bound.top()) / tileSize));
		glm::ivec2 endGrid = glm::vec2(std::ceil((bound.right() - startPos.x) / tileSize),
									   std::floor((startPos.y - bound.bottom()) / tileSize));

		for (int row = startGrid.y; row <= endGrid.y; row++)
		{
			for (int col = startGrid.x; col <= endGrid.x; col++)
			{
				glm::vec2 posGridCell =
					startPos + glm::vec2(static_cast<float>(col) * tileSize, -static_cast<float>(row) * tileSize);
				glm::vec2 sizeGridCell(tileSize);
				Rect gridCell(posGridCell, sizeGridCell);

				float intersect = CalculateIntersectionArea(bound, gridCell);
				float cellArea = tileSize * tileSize;
				if (intersect >= RoomConstants::INTERSECTION_THRESHOLD * cellArea)
				{
					MarkPosition(row, col, 1);
				}
			}
		}
	}
}

float GridSystem::CalculateIntersectionArea(const Rect &a, const Rect &b) const
{
	const float left = std::max(a.left(), b.left());
	const float right = std::min(a.right(), b.right());
	const float top = std::max(a.top(), b.top());
	const float bottom = std::min(a.bottom(), b.bottom());

	if (right <= left || top <= bottom)
		return 0.0f;

	return (right - left) * (top - bottom);
}

// ===== TerrainGenerator 實現 =====
TerrainGenerator::TerrainGenerator(std::weak_ptr<RoomObjectFactory> factory) : m_Factory(factory) {}

std::shared_ptr<nGameObject> TerrainGenerator::CreateWall(int row, int col, const RoomSpaceInfo &spaceInfo)
{
	auto factory = m_Factory.lock();
	if (!factory)
		return nullptr;

	glm::vec2 pos =
		Tool::RoomGridToWorld({col, row}, spaceInfo.m_TileSize, spaceInfo.m_WorldCoord, spaceInfo.m_RoomRegion);
	pos += glm::vec2(0, 1.5f);

	// 直接使用工廠的新方法
	return factory->createWall(row, col, pos);
}

std::shared_ptr<nGameObject> TerrainGenerator::CreateFloor(int row, int col, const RoomSpaceInfo &spaceInfo)
{
	auto factory = m_Factory.lock();
	if (!factory)
		return nullptr;

	glm::vec2 pos =
		Tool::RoomGridToWorld({col, row}, spaceInfo.m_TileSize, spaceInfo.m_WorldCoord, spaceInfo.m_RoomRegion);

	// 直接使用工廠的新方法
	return factory->createFloor(pos);
}

std::shared_ptr<nGameObject> TerrainGenerator::CreateDoor(int row, int col, const RoomSpaceInfo &spaceInfo)
{
	auto factory = m_Factory.lock();
	if (!factory)
		return nullptr;

	glm::vec2 pos =
		Tool::RoomGridToWorld({col, row}, spaceInfo.m_TileSize, spaceInfo.m_WorldCoord, spaceInfo.m_RoomRegion);

	// 直接使用工廠的新方法
	return factory->createDoor(pos);
}

// ===== DungeonRoom 實現 =====
DungeonRoom::DungeonRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader> &loader,
						 const std::shared_ptr<RoomObjectFactory> &room_object_factory, const glm::vec2 &mapGridPos,
						 const RoomType roomType) :
	Room(worldCoord, loader, room_object_factory), m_RoomType(roomType), m_MapGridPos(mapGridPos)
{
	// 初始化組合的管理器
	m_GridSystem = std::make_unique<GridSystem>();
	m_ConnectionManager = std::make_unique<RoomConnectionManager>();
	m_TerrainGenerator = std::make_unique<TerrainGenerator>(room_object_factory);
}

void DungeonRoom::Start(const std::shared_ptr<Character> &player)
{
	Room::Start(player);
	InitializeGrid();

	// 注意：碰撞優化將在 FinalizeRoomSetup() 中進行
	LOG_DEBUG("DungeonRoom::Start - Room basic setup completed at ({:.1f}, {:.1f})", m_RoomSpaceInfo.m_WorldCoord.x,
			  m_RoomSpaceInfo.m_WorldCoord.y);
}

void DungeonRoom::Update()
{
	Room::Update();
	DebugDungeonRoom();
}

void DungeonRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadStartingRoomObjectPosition();
	InitializeRoomObjects(jsonData);
}

void DungeonRoom::SetState(RoomState newState)
{
	if (m_State != newState)
	{
		m_State = newState;
		OnStateChanged();
	}
}

bool DungeonRoom::IsPlayerInsideRegion() const
{
	const auto player = m_Player.lock();
	if (!player)
		return false;

	const auto roomWorldCoord = m_RoomSpaceInfo.m_WorldCoord;
	const auto roomRegionInPixel = m_RoomSpaceInfo.m_RoomRegion * m_RoomSpaceInfo.m_TileSize;

	return (player->m_WorldCoord.x < roomWorldCoord.x + roomRegionInPixel.x / 2.0f) &&
		(player->m_WorldCoord.y < roomWorldCoord.y + roomRegionInPixel.y / 2.0f) &&
		(player->m_WorldCoord.x > roomWorldCoord.x - roomRegionInPixel.x / 2.0f) &&
		(player->m_WorldCoord.y > roomWorldCoord.y - roomRegionInPixel.y / 2.0f);
}

bool DungeonRoom::IsPlayerInsideRoom() const
{
	const auto player = m_Player.lock();
	if (!player)
		return false;

	const auto roomWorldCoord = m_RoomSpaceInfo.m_WorldCoord;
	const auto roomSizeInPixel = m_RoomSpaceInfo.m_RoomSize * m_RoomSpaceInfo.m_TileSize;
	constexpr float epsilon = 16.0f;

	return (player->m_WorldCoord.x < roomWorldCoord.x + (roomSizeInPixel.x / 2.0f - epsilon)) &&
		(player->m_WorldCoord.y < roomWorldCoord.y + (roomSizeInPixel.y / 2.0f - epsilon)) &&
		(player->m_WorldCoord.x > roomWorldCoord.x - (roomSizeInPixel.x / 2.0f - epsilon)) &&
		(player->m_WorldCoord.y > roomWorldCoord.y - (roomSizeInPixel.y / 2.0f - epsilon));
}

// 房間連接管理（委託給 RoomConnectionManager）
void DungeonRoom::SetNeighborRoom(Direction dir, const std::weak_ptr<DungeonRoom> &neighbor, bool hasConnection)
{
	m_ConnectionManager->SetNeighbor(dir, neighbor);
	m_ConnectionManager->SetConnection(dir, hasConnection);
}

std::weak_ptr<DungeonRoom> DungeonRoom::GetNeighborRoom(Direction dir) const
{
	return m_ConnectionManager->GetNeighbor(dir);
}

bool DungeonRoom::HasConnectionToDirection(Direction dir) const { return m_ConnectionManager->HasConnection(dir); }

std::vector<std::weak_ptr<DungeonRoom>> DungeonRoom::GetConnectedNeighbors() const
{
	return m_ConnectionManager->GetConnectedNeighbors();
}

std::vector<Direction> DungeonRoom::GetConnectedDirections() const
{
	return m_ConnectionManager->GetConnectedDirections();
}

// 網格系統訪問
const std::vector<std::vector<int>> &DungeonRoom::GetGridData() const { return m_GridSystem->GetGrid(); }

bool DungeonRoom::IsGridPositionBlocked(int row, int col) const { return m_GridSystem->IsPositionBlocked(row, col); }

void DungeonRoom::InitializeGrid()
{
	m_GridSystem->Initialize();
	m_GridSystem->UpdateGridFromObjects(m_RoomObjects, m_RoomSpaceInfo);
}

// 地形創建方法
void DungeonRoom::CreateWall(int row, int col)
{
	if (auto wall = m_TerrainGenerator->CreateWall(row, col, m_RoomSpaceInfo))
	{
		AddRoomObject(wall);
		m_GridSystem->MarkPosition(row, col, 1);
	}
}

void DungeonRoom::CreateFloor(int row, int col)
{
	if (auto floor = m_TerrainGenerator->CreateFloor(row, col, m_RoomSpaceInfo))
	{
		AddRoomObject(floor);
	}
}

void DungeonRoom::CreateDoor(int row, int col)
{
	if (auto door = m_TerrainGenerator->CreateDoor(row, col, m_RoomSpaceInfo))
	{
		AddRoomObject(door);
		m_Doors.emplace_back(door);
		m_GridSystem->MarkPosition(row, col, 1);
	}
}

void DungeonRoom::CreateCorridorInDirection(Direction dir)
{
	const glm::vec2 center = m_RoomSpaceInfo.m_RoomRegion / 2.0f;
	const glm::vec2 roomSize = m_RoomSpaceInfo.m_RoomSize;

	glm::ivec2 limitStart, limitEnd;
	glm::ivec2 doorStart, doorEnd;
	glm::vec2 delta;
	int doorLine = 0;

	switch (dir)
	{
	case Direction::UP:
		delta = {0, -1};
		limitStart = {center.x - RoomConstants::CORRIDOR_WIDTH / 2, 0};
		limitEnd = {center.x + RoomConstants::CORRIDOR_WIDTH / 2, center.y - roomSize.y / 2};
		doorLine = static_cast<int>(center.y) - static_cast<int>(roomSize.y / 2);
		doorStart = {center.x - (RoomConstants::CORRIDOR_WIDTH - 2) / 2, doorLine};
		doorEnd = {center.x + (RoomConstants::CORRIDOR_WIDTH - 2) / 2, doorLine};
		break;
	case Direction::DOWN:
		delta = {0, 1};
		limitStart = {center.x - RoomConstants::CORRIDOR_WIDTH / 2, center.y + roomSize.y / 2};
		limitEnd = {center.x + RoomConstants::CORRIDOR_WIDTH / 2, m_RoomSpaceInfo.m_RoomRegion.y};
		doorLine = static_cast<int>(center.y) + static_cast<int>(roomSize.y / 2);
		doorStart = {center.x - (RoomConstants::CORRIDOR_WIDTH - 2) / 2, doorLine};
		doorEnd = {center.x + (RoomConstants::CORRIDOR_WIDTH - 2) / 2, doorLine};
		break;
	case Direction::LEFT:
		delta = {-1, 0};
		limitStart = {0, center.y - RoomConstants::CORRIDOR_WIDTH / 2};
		limitEnd = {center.x - roomSize.x / 2, center.y + RoomConstants::CORRIDOR_WIDTH / 2};
		doorLine = static_cast<int>(center.x) - static_cast<int>(roomSize.x / 2);
		doorStart = {doorLine, center.y - (RoomConstants::CORRIDOR_WIDTH - 2) / 2};
		doorEnd = {doorLine, center.y + (RoomConstants::CORRIDOR_WIDTH - 2) / 2};
		break;
	case Direction::RIGHT:
		delta = {1, 0};
		limitStart = {center.x + roomSize.x / 2, center.y - RoomConstants::CORRIDOR_WIDTH / 2};
		limitEnd = {m_RoomSpaceInfo.m_RoomRegion.x, center.y + RoomConstants::CORRIDOR_WIDTH / 2};
		doorLine = static_cast<int>(center.x) + static_cast<int>(roomSize.x / 2);
		doorStart = {doorLine, center.y - (RoomConstants::CORRIDOR_WIDTH - 2) / 2};
		doorEnd = {doorLine, center.y + (RoomConstants::CORRIDOR_WIDTH - 2) / 2};
		break;
	}

	// 建通道地板和墻壁
	for (int row = limitStart.y; row < limitEnd.y; ++row)
	{
		for (int col = limitStart.x; col < limitEnd.x; ++col)
		{
			const bool isWall = (delta.x == 0) ? (col == limitStart.x || col == limitEnd.x - 1)
											   : (row == limitStart.y || row == limitEnd.y - 1);
			if (isWall)
			{
				CreateWall(row, col);
				continue;
			}
			CreateFloor(row, col);
		}
	}

	// 建門
	if (delta.x == 0)
	{
		for (int col = doorStart.x; col < doorEnd.x; ++col)
			CreateDoor(doorLine, col);
	}
	else
	{
		for (int row = doorStart.y; row < doorEnd.y; ++row)
			CreateDoor(row, doorLine);
	}
}

void DungeonRoom::CreateWallInDirection(Direction dir)
{
	const glm::vec2 center = m_RoomSpaceInfo.m_RoomRegion / 2.0f;
	const glm::vec2 roomSize = m_RoomSpaceInfo.m_RoomSize;

	glm::ivec2 wallStart, wallEnd;
	int doorLine = 0;

	switch (dir)
	{
	case Direction::UP:
		doorLine = static_cast<int>(center.y) - static_cast<int>(roomSize.y / 2);
		wallStart = {center.x - (RoomConstants::CORRIDOR_WIDTH - 2) / 2, doorLine};
		wallEnd = {center.x + (RoomConstants::CORRIDOR_WIDTH - 2) / 2, doorLine};
		break;
	case Direction::DOWN:
		doorLine = static_cast<int>(center.y) + static_cast<int>(roomSize.y / 2);
		wallStart = {center.x - (RoomConstants::CORRIDOR_WIDTH - 2) / 2, doorLine};
		wallEnd = {center.x + (RoomConstants::CORRIDOR_WIDTH - 2) / 2, doorLine};
		break;
	case Direction::LEFT:
		doorLine = static_cast<int>(center.x) - static_cast<int>(roomSize.x / 2);
		wallStart = {doorLine, center.y - (RoomConstants::CORRIDOR_WIDTH - 2) / 2};
		wallEnd = {doorLine, center.y + (RoomConstants::CORRIDOR_WIDTH - 2) / 2};
		break;
	case Direction::RIGHT:
		doorLine = static_cast<int>(center.x) + static_cast<int>(roomSize.x / 2);
		wallStart = {doorLine, center.y - (RoomConstants::CORRIDOR_WIDTH - 2) / 2};
		wallEnd = {doorLine, center.y + (RoomConstants::CORRIDOR_WIDTH - 2) / 2};
		break;
	}

	// 建牆
	if (dir == Direction::UP || dir == Direction::DOWN)
	{
		for (int col = wallStart.x; col < wallEnd.x; ++col)
			CreateWall(doorLine, col);
	}
	else
	{
		for (int row = wallStart.y; row < wallEnd.y; ++row)
			CreateWall(row, doorLine);
	}
}

void DungeonRoom::DebugDungeonRoom()
{
	ImGui::Begin("Current Room Grid Viewer Can't Spawn");

	ImVec2 tableSize = ImGui::GetContentRegionAvail();
	ImGui::BeginChild("TableContainer", tableSize, false, ImGuiWindowFlags_None);

	if (ImGui::BeginTable("RoomTable", RoomConstants::GRID_SIZE,
						  ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp))
	{
		for (int index = 0; index < RoomConstants::GRID_SIZE; index++)
			ImGui::TableSetupColumn((std::to_string(index)).c_str(), ImGuiTableColumnFlags_WidthStretch, tableSize.x);

		ImGui::TableHeadersRow();

		const auto &grid = m_GridSystem->GetGrid();
		for (int row = 0; row < RoomConstants::GRID_SIZE; row++)
		{
			ImGui::TableNextRow();
			for (int col = 0; col < RoomConstants::GRID_SIZE; col++)
			{
				ImGui::TableSetColumnIndex(col);
				if (grid[row][col])
				{
					ImGui::TextColored(ImVec4(0, 0, 1, 1), "1");
				}
				else
				{
					ImGui::TextColored(ImVec4(1, 1, 1, 1), "0");
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::EndChild();
	ImGui::End();
}

void DungeonRoom::FinalizeRoomSetup()
{
	// 重新初始化網格以包含所有新創建的物件
	InitializeGrid();

	// 在所有牆壁和通道創建完成後進行碰撞優化
	OptimizeWallCollisions();
}
