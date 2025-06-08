//
// Created by Assistant on 2025/1/20.
//

#include "Room/CollisionOptimizer.hpp"
#include <algorithm>
#include "Components/CollisionComponent.hpp"
#include "Override/nGameObject.hpp"
#include "RoomObject/DestructibleObject.hpp"
#include "RoomObject/WallObject.hpp"
#include "Structs/CollisionComponentStruct.hpp"
#include "Util/Logger.hpp"


bool CollisionOptimizer::IsWallObject(const std::shared_ptr<nGameObject> &obj) const
{
	// 檢查是否為 WallObject 類型
	if (auto wallObj = std::dynamic_pointer_cast<WallObject>(obj))
	{
		return true;
	}

	// 排除可破壞物件
	if (auto destructibleObj = std::dynamic_pointer_cast<DestructibleObject>(obj))
	{
		return false;
	}

	// 也可以通過類別名稱檢查（備用方案）
	const std::string className = obj->GetClassName();
	return className == "WallObject" ||
		(className == "nGameObject" && obj->GetName().find("wall") != std::string::npos);
}

std::vector<std::vector<bool>>
CollisionOptimizer::CreateWallGrid(const std::vector<std::shared_ptr<nGameObject>> &roomObjects, int gridSize,
								   const glm::vec2 &tileSize, const glm::vec2 &roomWorldCoord)
{
	std::vector<std::vector<bool>> wallGrid(gridSize, std::vector<bool>(gridSize, false));

	// 使用與 GridSystem::UpdateGridFromObjects 相同的方式
	const float gridSizeFloat = static_cast<float>(gridSize);

	// 計算左上角格子世界坐標軸（與 GridSystem::UpdateGridFromObjects 相同）
	auto startPos = glm::vec2(gridSizeFloat * tileSize.x / 2) - glm::vec2(tileSize.x / 2);
	startPos *= glm::vec2(-1, 1);
	startPos += roomWorldCoord;

	for (const auto &obj : roomObjects)
	{
		if (!IsWallObject(obj))
			continue;

		// 獲取物件的碰撞組件
		auto collisionComp = obj->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		if (!collisionComp || !collisionComp->IsActive())
			continue;

		// 獲取碰撞邊界
		Rect bound = collisionComp->GetBounds();

		// 計算網格範圍（與 GridSystem::UpdateGridFromObjects 相同）
		glm::ivec2 startGrid = glm::vec2(std::floor((bound.left() - startPos.x) / tileSize.x),
										 std::ceil((startPos.y - bound.top()) / tileSize.y));
		glm::ivec2 endGrid = glm::vec2(std::ceil((bound.right() - startPos.x) / tileSize.x),
									   std::floor((startPos.y - bound.bottom()) / tileSize.y));

		// 標記網格位置
		for (int row = startGrid.y; row <= endGrid.y; row++)
		{
			for (int col = startGrid.x; col <= endGrid.x; col++)
			{
				if (row >= 0 && row < gridSize && col >= 0 && col < gridSize)
				{
					// 計算交集面積來決定是否標記（與 GridSystem 相同）
					glm::vec2 posGridCell = startPos +
						glm::vec2(static_cast<float>(col) * tileSize.x, -static_cast<float>(row) * tileSize.y);
					glm::vec2 sizeGridCell(tileSize);
					Rect gridCell(posGridCell, sizeGridCell);

					float intersect = CalculateIntersectionArea(bound, gridCell);
					float cellArea = tileSize.x * tileSize.y;
					if (intersect >= 0.5f * cellArea) // RoomConstants::INTERSECTION_THRESHOLD
					{
						wallGrid[row][col] = true;
					}
				}
			}
		}
	}


	return wallGrid;
}

std::vector<CollisionRect> CollisionOptimizer::FindRectangularRegions(const std::vector<std::vector<bool>> &wallGrid,
																	  const glm::vec2 &tileSize,
																	  const glm::vec2 &roomWorldCoord)
{
	std::vector<CollisionRect> regions;
	int gridSize = static_cast<int>(wallGrid.size());
	std::vector<std::vector<bool>> visited(gridSize, std::vector<bool>(gridSize, false));


	for (int row = 0; row < gridSize; ++row)
	{
		for (int col = 0; col < gridSize; ++col)
		{
			if (!wallGrid[row][col] || visited[row][col])
				continue;

			// 尋找最大的矩形區域
			int maxWidth = 0;
			int maxHeight = 0;

			// 先找到這一行的最大寬度
			for (int c = col; c < gridSize && wallGrid[row][c] && !visited[row][c]; ++c)
			{
				maxWidth++;
			}

			// 然後向下擴展，找到最大高度
			for (int r = row; r < gridSize; ++r)
			{
				bool canExtend = true;
				for (int c = col; c < col + maxWidth; ++c)
				{
					if (!wallGrid[r][c] || visited[r][c])
					{
						canExtend = false;
						break;
					}
				}

				if (canExtend)
					maxHeight++;
				else
					break;
			}

			// 標記這個區域為已訪問
			for (int r = row; r < row + maxHeight; ++r)
			{
				for (int c = col; c < col + maxWidth; ++c)
				{
					visited[r][c] = true;
				}
			}

			// 創建碰撞矩形
			CollisionRect rect;
			rect.topLeft = {col, row};
			rect.bottomRight = {col + maxWidth - 1, row + maxHeight - 1};

			// 計算矩形左上角的世界座標
			glm::vec2 topLeftWorld = GridToWorldCoord({col, row}, tileSize, roomWorldCoord);

			// 計算矩形中心點的世界座標
			glm::vec2 centerOffset = {(maxWidth - 1) * tileSize.x / 2.0f, -(maxHeight - 1) * tileSize.y / 2.0f};
			rect.worldPos = topLeftWorld + centerOffset;
			rect.size = {maxWidth * tileSize.x, maxHeight * tileSize.y};

			regions.push_back(rect);
		}
	}


	return regions;
}

glm::vec2 CollisionOptimizer::GridToWorldCoord(const glm::ivec2 &gridPos, const glm::vec2 &tileSize,
											   const glm::vec2 &roomWorldCoord) const
{
	// 使用與 Tool::RoomGridToWorld 完全相同的計算方式
	const float gridSize = 35.0f; // RoomConstants::GRID_SIZE
	glm::vec2 roomRegion = {gridSize, gridSize};

	glm::vec2 start = glm::vec2(roomRegion * tileSize / 2.0f) - tileSize / 2.0f;
	start *= glm::vec2(-1, 1);
	start += roomWorldCoord;

	glm::vec2 worldPos =
		start + glm::vec2(static_cast<float>(gridPos.x) * tileSize.x, -static_cast<float>(gridPos.y) * tileSize.y);

	return worldPos;
}

float CollisionOptimizer::CalculateIntersectionArea(const Rect &a, const Rect &b) const
{
	const float left = std::max(a.left(), b.left());
	const float right = std::min(a.right(), b.right());
	const float top = std::max(a.top(), b.top());
	const float bottom = std::min(a.bottom(), b.bottom());

	if (right <= left || top <= bottom)
		return 0.0f;

	return (right - left) * (top - bottom);
}

std::vector<CollisionRect>
CollisionOptimizer::OptimizeWallCollisions(const std::vector<std::shared_ptr<nGameObject>> &roomObjects,
										   const glm::vec2 &tileSize, const glm::vec2 &roomWorldCoord, int gridSize)
{
	// 1. 創建牆壁網格
	auto wallGrid = CreateWallGrid(roomObjects, gridSize, tileSize, roomWorldCoord);

	// 2. 尋找連續的矩形區域
	auto regions = FindRectangularRegions(wallGrid, tileSize, roomWorldCoord);

	return regions;
}
