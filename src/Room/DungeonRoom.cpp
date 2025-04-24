//
// Created by QuzzS on 2025/4/12.
//

#include "Room/DungeonRoom.hpp"

#include <iostream>
#include <map>

#include "Camera.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/DoorComponent.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Renderer.hpp"

#include "Creature/Character.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Loader.hpp"
#include "Tool/Tool.hpp"

void DungeonRoom::Start(const std::shared_ptr<Character> &player)
{
	Room::Start(player);

	CreateGrid();
}

void DungeonRoom::Update()
{
	Room::Update();
}

void DungeonRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadStartingRoomObjectPosition();
	InitializeRoomObjects(jsonData);
}

bool DungeonRoom::IsPlayerInsideRegion() const
{
	const auto player = m_Player.lock();
	if (!player) return false;
	const auto roomWorldCoord = m_RoomSpaceInfo.m_WorldCoord;
	const auto roomRegionInPixel = m_RoomSpaceInfo.m_RoomRegion * m_RoomSpaceInfo.m_TileSize;
	return (player->m_WorldCoord.x < roomWorldCoord.x + roomRegionInPixel.x/2.0f) &&
	   (player->m_WorldCoord.y < roomWorldCoord.y + roomRegionInPixel.y/2.0f) &&
	   (player->m_WorldCoord.x > roomWorldCoord.x - roomRegionInPixel.x/2.0f) &&
	   (player->m_WorldCoord.y > roomWorldCoord.y - roomRegionInPixel.y/2.0f);
}

bool DungeonRoom::IsPlayerInsideRoom() const
{
	const auto player = m_Player.lock();
	if (!player) return false;
	const auto roomWorldCoord = m_RoomSpaceInfo.m_WorldCoord;
	const auto roomSizeInPixel = m_RoomSpaceInfo.m_RoomSize * m_RoomSpaceInfo.m_TileSize;
	constexpr float epsilon = 16.0f; // 防止被鎖在門外 TODO:範圍有點奇怪，向下偏了
	return (player->m_WorldCoord.x < roomWorldCoord.x + (roomSizeInPixel.x/2.0f - epsilon)) &&
	   (player->m_WorldCoord.y < roomWorldCoord.y + (roomSizeInPixel.y/2.0f - epsilon)) &&
	   (player->m_WorldCoord.x > roomWorldCoord.x - (roomSizeInPixel.x/2.0f - epsilon)) &&
	   (player->m_WorldCoord.y > roomWorldCoord.y - (roomSizeInPixel.y/2.0f - epsilon));
}

void DungeonRoom::CreateGrid()
{
	const float gridSize = m_RoomSpaceInfo.m_RoomRegion.x;
	const float tileSize = m_RoomSpaceInfo.m_TileSize.x;

	// 計算左上角格子世界坐標軸
	auto startPos = glm::vec2(gridSize * tileSize/2) - glm::vec2(tileSize / 2);
	startPos *= glm::vec2(-1,1);
	startPos += m_RoomSpaceInfo.m_WorldCoord; // 轉移中心點

	for (const auto& elem: m_RoomObjects)
	{
		auto collisionComp = elem->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		if (!collisionComp) continue;
		Rect bound = collisionComp->GetBounds();
		glm::ivec2 startGrid = glm::vec2( std::floor((bound.left() - startPos.x) / tileSize), std::ceil((startPos.y - bound.top())/ tileSize) );
		glm::ivec2 endGrid = glm::vec2( std::ceil((bound.right() - startPos.x) / tileSize), std::floor((startPos.y - bound.bottom()) / tileSize) );
		for (int row = startGrid.y; row <= endGrid.y; row++)
		{
			for (int col = startGrid.x; col <= endGrid.x; col++)
			{
				glm::vec2 posGridCell = startPos + glm::vec2(static_cast<float>(col) * tileSize, -static_cast<float>(row) * tileSize);
				glm::vec2 sizeGridCell(tileSize);
				Rect gridCell(posGridCell, sizeGridCell);

				float intersect = IntersectionArea(bound, gridCell);
				float cellArea = tileSize * tileSize;
				if (intersect >= 0.5f * cellArea)
				{
					m_Mark[row][col] = 1;
				}
			}
		}
	}
}

float DungeonRoom::IntersectionArea(const Rect& a, const Rect& b) {
	const float left   = std::max(a.left(),   b.left());
	const float right  = std::min(a.right(),  b.right());
	const float top    = std::max(a.top(),    b.top());
	const float bottom = std::min(a.bottom(), b.bottom());

	if (right <= left || top <= bottom)
		return 0.0f; // 無交集

	return (right - left) * (top - bottom);
}

void DungeonRoom::CreateCorridorInDirection(Direction dir)
{
    constexpr float corridorWidth = 7.0f;
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
        limitStart = {center.x - corridorWidth / 2, 0};
        limitEnd = {center.x + corridorWidth / 2, center.y - roomSize.y / 2};
    	doorLine = static_cast<int>(center.y) - static_cast<int>(roomSize.y / 2); // 直接捨去小數-1變index0~34
        doorStart = {center.x - (corridorWidth - 2) / 2, doorLine};
        doorEnd = {center.x + (corridorWidth - 2) / 2, doorLine};
        break;
    case Direction::DOWN:
        delta = {0, 1};
        limitStart = {center.x - corridorWidth / 2, center.y + roomSize.y / 2};
        limitEnd = {center.x + corridorWidth / 2, m_RoomSpaceInfo.m_RoomRegion.y};
        doorLine = static_cast<int>(center.y) + static_cast<int>(roomSize.y / 2);
        doorStart = {center.x - (corridorWidth - 2) / 2, doorLine};
        doorEnd = {center.x + (corridorWidth - 2) / 2, doorLine};
        break;
    case Direction::LEFT:
        delta = {-1, 0};
        limitStart = {0, center.y - corridorWidth / 2};
        limitEnd = {center.x - roomSize.x / 2, center.y + corridorWidth / 2};
    	doorLine = static_cast<int>(center.x) - static_cast<int>(roomSize.x / 2);
        doorStart = {doorLine, center.y - (corridorWidth - 2) / 2};
        doorEnd = {doorLine, center.y + (corridorWidth - 2) / 2};
        break;
    case Direction::RIGHT:
        delta = {1, 0};
        limitStart = {center.x + roomSize.x / 2, center.y - corridorWidth / 2};
        limitEnd = {m_RoomSpaceInfo.m_RoomRegion.x, center.y + corridorWidth / 2};
    	doorLine = static_cast<int>(center.x) + static_cast<int>(roomSize.x / 2);
        doorStart = {doorLine, center.y - (corridorWidth - 2) / 2};
        doorEnd = {doorLine, center.y + (corridorWidth - 2) / 2};
        break;
    }

	// 建通道地板和墻壁
    for (int row = limitStart.y; row < limitEnd.y; ++row)
    {
        for (int col = limitStart.x; col < limitEnd.x; ++col)
        {
            const bool isWall = (delta.x == 0) ? (col == limitStart.x || col == limitEnd.x - 1)
                                              : (row == limitStart.y || row == limitEnd.y - 1);
            if(isWall)
            {
	            CreateWall(row, col);
            	continue;
            }
        	CreateFloor(row,col);
        }
    }

    // 建門
    if (delta.x == 0)
    {
        for (int col = doorStart.x; col < doorEnd.x; ++col) CreateDoor(doorLine, col);
    }
    else
    {
        for (int row = doorStart.y; row < doorEnd.y; ++row) CreateDoor(row, doorLine);
    }
}

void DungeonRoom::CreateWallInDirection(Direction dir)
{
	constexpr float corridorWidth = 7.0f;
    const glm::vec2 center = m_RoomSpaceInfo.m_RoomRegion / 2.0f;
    const glm::vec2 roomSize = m_RoomSpaceInfo.m_RoomSize;

    glm::ivec2 wallStart, wallEnd;
    glm::vec2 delta;
    int doorLine = 0;

    switch (dir)
    {
    case Direction::UP:
        delta = {0, -1};
    	doorLine = static_cast<int>(center.y) - static_cast<int>(roomSize.y / 2); // 直接捨去小數-1變index0~34
        wallStart = {center.x - (corridorWidth - 2) / 2, doorLine};
        wallEnd = {center.x + (corridorWidth - 2) / 2, doorLine};
        break;
    case Direction::DOWN:
        delta = {0, 1};
        doorLine = static_cast<int>(center.y) + static_cast<int>(roomSize.y / 2);
        wallStart = {center.x - (corridorWidth - 2) / 2, doorLine};
        wallEnd = {center.x + (corridorWidth - 2) / 2, doorLine};
        break;
    case Direction::LEFT:
        delta = {-1, 0};
    	doorLine = static_cast<int>(center.x) - static_cast<int>(roomSize.x / 2);
        wallStart = {doorLine, center.y - (corridorWidth - 2) / 2};
        wallEnd = {doorLine, center.y + (corridorWidth - 2) / 2};
        break;
    case Direction::RIGHT:
        delta = {1, 0};
    	doorLine = static_cast<int>(center.x) + static_cast<int>(roomSize.x / 2);
        wallStart = {doorLine, center.y - (corridorWidth - 2) / 2};
        wallEnd = {doorLine, center.y + (corridorWidth - 2) / 2};
        break;
    }

    // 建門
    if (delta.x == 0)
    {
        for (int col = wallStart.x; col < wallEnd.x; ++col) CreateWall(doorLine, col);
    }
    else
    {
        for (int row = wallStart.y; row < wallEnd.y; ++row) CreateWall(row, doorLine);
    }
}

void DungeonRoom::CreateWall(int row, int col)
{
    auto factory = m_Factory.lock();
    glm::vec2 pos = Tool::RoomGridToWorld({col, row}, m_RoomSpaceInfo.m_TileSize, m_RoomSpaceInfo.m_WorldCoord, m_RoomSpaceInfo.m_RoomRegion);

    auto wall = factory->createRoomObject("w604", "Wall");
	wall->SetPosOffset(wall->GetPosOffset() + glm::vec2{0,1.5f}); // 手動調整
    wall->SetWorldCoord(pos);
    AddRoomObject(wall);

    m_Mark[row][col] = 1;
}

void DungeonRoom::CreateFloor(int row, int col)
{
	auto factory = m_Factory.lock();
	glm::vec2 pos = Tool::RoomGridToWorld({col, row}, m_RoomSpaceInfo.m_TileSize, m_RoomSpaceInfo.m_WorldCoord, m_RoomSpaceInfo.m_RoomRegion);

	auto object = factory->createRoomObject("f601", "Floor");
	object->SetWorldCoord(pos);
	AddRoomObject(object);
}

void DungeonRoom::CreateDoor(int row, int col)
{
    auto factory = m_Factory.lock();
    auto camera = m_CachedCamera.lock();
    auto root = m_CachedRenderer.lock();

	m_Mark[row][col] = 1;

    glm::vec2 pos = Tool::RoomGridToWorld({col, row}, m_RoomSpaceInfo.m_TileSize, m_RoomSpaceInfo.m_WorldCoord, m_RoomSpaceInfo.m_RoomRegion);
    auto door = factory->createRoomObject("object_door_0", "door");
    door->SetWorldCoord(pos);
    AddRoomObject(door);
    m_Doors.emplace_back(door);
}

void DungeonRoom::DebugDungeonRoom()
{
	// ImGui::Begin("Door Position Offset test");
	// static glm::vec2 pos = {0,0};
	// ImGui::InputFloat("positionX", &pos[0],1.0f, 1.0f, "%.3f");
	// ImGui::InputFloat("positionY", &pos[1],1.0f, 1.0f, "%.3f");
	// static glm::vec2 pos2 = {0,0};
	// ImGui::InputFloat("positionColliderX", &pos2[0],1.0f, 1.0f, "%.3f");
	// ImGui::InputFloat("positionColliderY", &pos2[1],1.0f, 1.0f, "%.3f");
	// m_Door->SetWorldCoord(pos);
	// if (ImGui::Button("Door"))
	// {
	// 	m_Door->SetWorldCoord(pos);
	// }
	// const auto collider = m_Door->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	// ImGui::End();
}




