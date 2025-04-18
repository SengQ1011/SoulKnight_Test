//
// Created by QuzzS on 2025/4/12.
//

#include "Room/DungeonRoom.hpp"
#include "Camera.hpp"
#include "Util/Renderer.hpp"
#include <execution>

void DungeonRoom::Start(const std::shared_ptr<Character> &player)
{
	Room::Start(player);
	auto time1 = std::chrono::high_resolution_clock::now();
	CreateGridAndVisibleGrid();
	auto time2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1);
	LOG_DEBUG("Create grid {}us", duration.count());

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
	auto bound3535 = m_Factory.lock()->createRoomObject("bound3535","nGameObject");
	glm::vec2 pos = m_RoomSpaceInfo.m_WorldCoord;
	bound3535->SetWorldCoord(pos);
	bound3535->SetZIndex(20.5);
	m_CachedCamera.lock()->AddChild(bound3535);
	m_CachedRenderer.lock()->AddChild(bound3535);
	m_Bound3535 = bound3535;

	for (int row = 0; row < 35; row++)
	{
		for (int col = 0; col < 35; col++)
		{
			if (m_Mark[row][col] == 1)
			{
				auto bound = std::make_shared<nGameObject>();
				bound->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/BlueCollider.png"));
				glm::vec2 pos = startPos + glm::vec2(static_cast<float>(col) * tileSize, -static_cast<float>(row) * tileSize);
				bound->SetInitialScale(glm::vec2(16));
				bound->SetInitialScaleSet(true);
				bound->SetWorldCoord(pos);
				bound->SetZIndexType(CUSTOM);
				bound->SetZIndex(100);
				m_CachedCamera.lock()->AddChild(bound);
				m_CachedRenderer.lock()->AddChild(bound);
				m_Grid[row][col] = bound;
			}
		}
	}
}

void DungeonRoom::CreateGridAndVisibleGridPar()
{
	const float gridSize = m_RoomSpaceInfo.m_RoomRegion.x;
	const float tileSize = m_RoomSpaceInfo.m_TileSize.x;

	// 計算左上角格子世界坐標軸
	auto startPos = glm::vec2(gridSize * tileSize/2) - glm::vec2(tileSize / 2);
	startPos *= glm::vec2(-1,1);
	startPos += m_RoomSpaceInfo.m_WorldCoord; // 轉移中心點

	std::mutex markMutex;

	std::for_each(std::execution::par, m_RoomObjects.begin(), m_RoomObjects.end(), [&](const std::shared_ptr<nGameObject>& elem)
	{
		auto collisionComp = elem->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		if (!collisionComp) return;

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
					std::scoped_lock lock(markMutex);
					m_Mark[row][col] = 1;
				}
			}
		}
	});

	for (int row = 0; row < 35; row++)
	{
		for (int col = 0; col < 35; col++)
		{
			if (m_Mark[row][col] == 0)
			{
				auto bound = m_Factory.lock()->createRoomObject("bound","nGameObject");
				glm::vec2 pos = startPos + glm::vec2(static_cast<float>(col) * tileSize, -static_cast<float>(row) * tileSize);
				bound->SetWorldCoord(pos);
				bound->SetZIndex(20);
				m_CachedCamera.lock()->AddChild(bound);
				m_CachedRenderer.lock()->AddChild(bound);
				m_Grid[row][col] = bound;
			}
			if (m_Mark[row][col] == 1)
			{
				auto bound = std::make_shared<nGameObject>();
				bound->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/BlueCollider.png"));
				glm::vec2 pos = startPos + glm::vec2(static_cast<float>(col) * tileSize, -static_cast<float>(row) * tileSize);
				bound->SetInitialScale(glm::vec2(16));
				bound->SetInitialScaleSet(true);
				bound->SetWorldCoord(pos);
				bound->SetZIndexType(CUSTOM);
				bound->SetZIndex(100);
				m_CachedCamera.lock()->AddChild(bound);
				m_CachedRenderer.lock()->AddChild(bound);
				m_Grid[row][col] = bound;
			}
		}
	}
}

float DungeonRoom::IntersectionArea(const Rect& a, const Rect& b) {
	float left   = std::max(a.left(),   b.left());
	float right  = std::min(a.right(),  b.right());
	float top    = std::max(a.top(),    b.top());
	float bottom = std::min(a.bottom(), b.bottom());

	if (right <= left || top <= bottom)
		return 0.0f; // 無交集

	return (right - left) * (top - bottom);
}


