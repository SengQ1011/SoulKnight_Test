//
// Created by QuzzS on 2025/4/12.
//

#include "Room/DungeonRoom.hpp"

#include <iostream>
#include <map>

#include "Camera.hpp"
#include "Components/CollisionComponent.hpp"
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

	CreateGridAndVisibleGrid();
	CreateCorridorInDirection(Direction::UP);
	CreateCorridorInDirection(Direction::DOWN);
	CreateCorridorInDirection(Direction::LEFT);
	CreateCorridorInDirection(Direction::RIGHT);

	// for (int row = 0; row < 35; row++)
	// {
	// 	for (int col = 0; col < 35; col++)
	// 	{
	// 		std::cout << m_Mark[row][col] << " ";
	// 	}
	// 	std::cout << std::endl;
	// }

}

void DungeonRoom::Update()
{
	Room::Update();

	if (Util::Input::IsKeyUp(Util::Keycode::O))
	{
		for (const auto& row: m_Grid)
		{
			for (const auto& elem: row)
			{
				if (!elem) continue;
				elem->SetVisible(true ^ elem->IsVisible());
			}
		}
		m_Bound3535->SetVisible(true ^ m_Bound3535->IsVisible());
	}
}

void DungeonRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadStartingRoomObjectPosition();
	InitializeRoomObjects(jsonData);
}

bool DungeonRoom::IsPlayerInside() const
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

void DungeonRoom::CreateGridAndVisibleGrid()
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

	// 下面都是测试可视化用的，可以删
	// auto bound3535 = m_Factory.lock()->createRoomObject("bound3535","nGameObject");
	// glm::vec2 pos = m_RoomSpaceInfo.m_WorldCoord;
	// bound3535->SetWorldCoord(pos);
	// bound3535->SetZIndex(20.5);
	// m_CachedCamera.lock()->AddChild(bound3535);
	// m_CachedRenderer.lock()->AddChild(bound3535);
	// m_Bound3535 = bound3535;

	// for (int row = 0; row < 35; row++)
	// {
	// 	for (int col = 0; col < 35; col++)
	// 	{
	// 		if (m_Mark[row][col] == 1)
	// 		{
	// 			auto bound = std::make_shared<nGameObject>();
	// 			bound->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/BlueCollider.png"));
	// 			glm::vec2 pos = startPos + glm::vec2(static_cast<float>(col) * tileSize, -static_cast<float>(row) * tileSize);
	// 			bound->SetInitialScale(glm::vec2(16));
	// 			bound->SetInitialScaleSet(true);
	// 			bound->SetWorldCoord(pos);
	// 			bound->SetZIndexType(CUSTOM);
	// 			bound->SetZIndex(100);
	// 			m_CachedCamera.lock()->AddChild(bound);
	// 			m_CachedRenderer.lock()->AddChild(bound);
	// 			m_Grid[row][col] = bound;
	// 		}
	// 	}
	// }
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
	const glm::vec2 gridCount = m_RoomSpaceInfo.m_RoomRegion;
	const glm::vec2 center = gridCount / 2.0f;
	const glm::vec2 roomSize = m_RoomSpaceInfo.m_RoomSize;

	const glm::vec2 tileSize = m_RoomSpaceInfo.m_TileSize;
	glm::vec2 topLeftWorldCoord = (gridCount - glm::vec2(1)) * tileSize / 2.0f ;
	topLeftWorldCoord *= glm::vec2(-1,1);
	topLeftWorldCoord += m_RoomSpaceInfo.m_WorldCoord; // 轉移中心點

	// 根據方向決定偏移方向與邊界終點
	glm::ivec2 delta; // 通道方向單位向量
	glm::ivec2 limitStart, limitEnd;
	switch (dir)
	{
	case Direction::UP:
		delta = glm::vec2(0, -1);
		limitStart = glm::ivec2(center.x - corridorWidth / 2, 0);
		limitEnd   = glm::ivec2(center.x + corridorWidth / 2, center.y - roomSize.y/2);
		break;
	case Direction::DOWN:
		delta = glm::vec2(0, 1);
		limitStart = glm::ivec2(center.x - corridorWidth / 2, center.y + roomSize.y/2);
		limitEnd   = glm::ivec2(center.x + corridorWidth / 2, gridCount.y);
		break;
	case Direction::LEFT:
		delta = glm::vec2(-1, 0);
		limitStart = glm::ivec2(0, center.y - corridorWidth / 2);
		limitEnd   = glm::ivec2(center.x - roomSize.x/2, center.y + corridorWidth / 2);
		break;
	case Direction::RIGHT:
		delta = glm::vec2(1, 0);
		limitStart = glm::ivec2(center.x + roomSize.x/2, center.y - corridorWidth / 2);
		limitEnd = glm::ivec2(gridCount.x, center.y + corridorWidth / 2);
		break;
	}
	if (delta.x == 0)
	{
		for (int row = limitStart.y; row < limitEnd.y; row++)
		{
			for (int col = limitStart.x; col < limitEnd.x; col++)
			{
				// TODO：墙壁动态建构的要手动调位置也 要加个offset吗？
				m_Mark[row][col] = 1;
				glm::vec2 pos = topLeftWorldCoord + glm::vec2(static_cast<float>(col) * tileSize.x, -static_cast<float>(row) * tileSize.y);
				std::shared_ptr<nGameObject> object;
				if (col == limitStart.x || col == limitEnd.x - 1)
				{
					pos += glm::vec2(0,1.5f);
					object = m_Factory.lock()->createRoomObject("w604","Wall");
				}
				else object = m_Factory.lock()->createRoomObject("f601","Floor");


				object->SetWorldCoord(pos);
				m_CachedCamera.lock()->AddChild(object);
				m_CachedRenderer.lock()->AddChild(object);
				AddRoomObject(object);
			}
		}
	}
	else if (delta.y == 0)
	{
		for (int col = limitStart.x; col < limitEnd.x; col++)
		{
			for (int row = limitStart.y; row < limitEnd.y; row++)
			{
				m_Mark[row][col] = 1;
				glm::vec2 pos = topLeftWorldCoord + glm::vec2(static_cast<float>(col) * tileSize.x, -static_cast<float>(row) * tileSize.y);
				std::shared_ptr<nGameObject> object;
				if (row == limitStart.y || row == limitEnd.y - 1)
				{
					pos += glm::vec2(0,1.5f);
					object = m_Factory.lock()->createRoomObject("w604","Wall");
				}
				else object = m_Factory.lock()->createRoomObject("f601","Floor");

				object->SetWorldCoord(pos);
				m_CachedCamera.lock()->AddChild(object);
				m_CachedRenderer.lock()->AddChild(object);
				AddRoomObject(object);
			}
		}
	}
}



