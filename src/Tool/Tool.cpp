//
// Created by QuzzS on 2025/2/25.
//

#include "Tool/Tool.hpp"

#include <iostream>
#include "Util/Input.hpp"
#include "Components/AiComponent.hpp"
#include "config.hpp"

#include "Components/AiComponent.hpp"

namespace Tool
{
	Rectangle Bound(glm::vec2 center, glm::vec2 size)
	{
		Rectangle Rect{};
		Rect.v_Max = center + size * 0.5f;
		Rect.v_Min = center - size * 0.5f;
		return Rect;
	}


	glm::vec2 Sdl2Glm(float x, float y)
	{
		glm::vec2 glmCoord;
		glmCoord.x = PTSD_Config::WINDOW_WIDTH * 0.5f - x;
		glmCoord.y = y - PTSD_Config::WINDOW_HEIGHT * 0.5f;
		return glmCoord;
	};

	glm::vec2 GetMouseCoord()
	{
		glm::vec2 mouse_coord = Util::Input::GetCursorPosition();
		// mouse_coord *= glm::vec2(1,1);
		return mouse_coord;
	}

	// 世界座標轉地圖格子座標
	glm::ivec2 WorldToMapGrid(const glm::vec2& worldPos, const glm::vec2& tileSize, const glm::vec2& roomRegion, const glm::ivec2& roomNum) {
		const glm::vec2 roomSize = tileSize * roomRegion;
		const glm::vec2 halfMap = glm::floor(glm::vec2(roomNum) / 2.0f);
		const glm::vec2 startWorld = roomSize * halfMap * glm::vec2(-1, 1);

		glm::vec2 gridPos = (worldPos - startWorld) / roomSize;
		gridPos *= glm::vec2(1, -1);  // Y 軸翻轉

		return {glm::floor(gridPos)};
	}

	// 地圖格子座標轉世界座標
	glm::vec2 MapGridToWorld(const glm::ivec2& gridPos, const glm::vec2& tileSize, const glm::vec2& roomRegion, const glm::ivec2& roomNum) {
		const glm::vec2 roomSize = tileSize * roomRegion;
		const glm::vec2 halfMap = glm::floor(glm::vec2(roomNum) / 2.0f);
		const glm::vec2 startWorld = roomSize * halfMap * glm::vec2(-1, 1);

		const glm::vec2 pos = glm::vec2(gridPos) * glm::vec2(1, -1) * roomSize + startWorld;
		return pos;
	}

	// 世界座標轉房間格子座標
	glm::ivec2 WorldToRoomGrid(const glm::vec2& worldPos, const glm::vec2& tileSize, const glm::vec2& roomCoord, const glm::vec2& roomRegion) {
		glm::vec2 start = glm::vec2(roomRegion * tileSize / 2.0f) - tileSize / 2.0f;
		start *= glm::vec2(-1, 1);
		start += roomCoord;

		const float x = (worldPos.x - start.x) / tileSize.x;
		const float y = (start.y - worldPos.y) / tileSize.y;

		return {glm::floor(x), glm::floor(y)};
	}

	// 房間格子座標轉世界座標
	glm::vec2 RoomGridToWorld(const glm::ivec2& grid, const glm::vec2& tileSize, const glm::vec2& roomCoord, const glm::vec2& roomRegion) {
		glm::vec2 start = glm::vec2(roomRegion * tileSize / 2.0f) - tileSize / 2.0f;
		start *= glm::vec2(-1, 1);
		start += roomCoord;

		return start + glm::vec2(static_cast<float>(grid.x) * tileSize.x, -static_cast<float>(grid.y) * tileSize.y);
	}

	glm::ivec2 FindIndexWhichGridObjectIsInside(const glm::vec2 &regionGridCount,
											const glm::vec2 &unitGridSize,
											const glm::vec2 &objectWorldCoord,
											const glm::vec2 &regionCenterPointWorldCoord)
	{
		glm::vec2 topLeftWorldCoord = unitGridSize * regionGridCount / 2.0f;
		topLeftWorldCoord *= glm::vec2(-1.0f, 1.0f);
		topLeftWorldCoord += regionCenterPointWorldCoord;

		glm::vec2 offset = (objectWorldCoord - topLeftWorldCoord) * glm::vec2(1, -1);
		glm::vec2 gridFloat = offset / unitGridSize;
		glm::ivec2 regionIndex = glm::ivec2(floor(gridFloat));

		if (regionIndex.x < 0 || regionIndex.x >= static_cast<int>(regionGridCount.x) ||
			regionIndex.y < 0 || regionIndex.y >= static_cast<int>(regionGridCount.y))
		{
			return {-1, -1};
		}

		return regionIndex;
	}

}