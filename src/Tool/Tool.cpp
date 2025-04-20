//
// Created by QuzzS on 2025/2/25.
//

#include "Tool/Tool.hpp"

#include "config.hpp"
#include <iostream>

namespace Tool
{
	Rectangle Bound(glm::vec2 center, glm::vec2 size)
	{
		Rectangle Rect{};
		Rect.v_Max = center + size * 0.5f;
		Rect.v_Min = center - size * 0.5f;
		return Rect;
	}


	glm::vec2 Sdl2Glm(int x, int y)
	{
		glm::vec2 glmCoord;
		glmCoord.x = WINDOW_WIDTH * 0.5f - x;
		glmCoord.y = y - WINDOW_HEIGHT * 0.5f;
		return glmCoord;
	};

	glm::vec2 GetMouseCoord()
	{
		int x, y;
		glm::vec2 glmCoord;
		SDL_GetMouseState(&x,&y);
		glmCoord = Sdl2Glm(x,y);
		return glmCoord;
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

	// glm::ivec2 FindIndexWhichGridObjectIsInside(const glm::vec2 &regionGridCount,
	// 										   const glm::vec2 &unitGridSize,
	// 										   const glm::vec2 &objectWorldCoord,
	// 										   const glm::vec2 &regionCenterPointWorldCoord)
	// {
	// 	// 计算区域左上角（阵列最低点）的世界坐标
	// 	glm::vec2 topLeftWorldCoord  = unitGridSize * regionGridCount / 2.0f; // 先找右上角
	// 	topLeftWorldCoord  *= glm::vec2(-1,1); // 换成左上角
	// 	topLeftWorldCoord  += regionCenterPointWorldCoord; // 若是特定房间则转移中心点，没有则中心不变
	//
	// 	// 计算以左上角为参考的坐标
	// 	const glm::vec2 offset = (objectWorldCoord - topLeftWorldCoord) * glm::vec2(1, -1);
	//
	// 	// const glm::ivec2 regionIndex = glm::vec2(std::floor(offset.x/unitGridSize.x), std::floor(offset.y/unitGridSize.y));
	// 	const glm::ivec2 regionIndex = glm::vec2(floor(offset/unitGridSize));
	// 	if (regionIndex.x < 0 || regionIndex.x >= static_cast<int>(regionGridCount.x) ||
	// 		regionIndex.y < 0 || regionIndex.y >= static_cast<int>(regionGridCount.y))
	// 	{
	// 		LOG_DEBUG("FindIndexWhichGridObjectIsInside returned false");
	// 		return {-1, -1};
	// 	}
	// 	return regionIndex;
	// }
	glm::ivec2 FindIndexWhichGridObjectIsInside(const glm::vec2 &regionGridCount,
											const glm::vec2 &unitGridSize,
											const glm::vec2 &objectWorldCoord,
											const glm::vec2 &regionCenterPointWorldCoord)
	{
		LOG_DEBUG("HELLO");
		glm::vec2 topLeftWorldCoord = unitGridSize * regionGridCount / 2.0f;
		topLeftWorldCoord *= glm::vec2(-1.0f, 1.0f);
		topLeftWorldCoord += regionCenterPointWorldCoord;

		glm::vec2 offset = (objectWorldCoord - topLeftWorldCoord) * glm::vec2(1, -1);
		glm::vec2 gridFloat = offset / unitGridSize;
		glm::ivec2 regionIndex = glm::ivec2(floor(gridFloat));

		// ✅ Debug 輸出
		LOG_DEBUG("========== FindIndex Debug ==========");
		LOG_DEBUG("regionGridCount: {}, {}", regionGridCount.x, regionGridCount.y);
		LOG_DEBUG("unitGridSize: {}, {}", unitGridSize.x, unitGridSize.y);
		LOG_DEBUG("regionCenterPointWorldCoord: {}, {}", regionCenterPointWorldCoord.x, regionCenterPointWorldCoord.y);
		LOG_DEBUG("objectWorldCoord: {}, {}", objectWorldCoord.x, objectWorldCoord.y);
		LOG_DEBUG("topLeftWorldCoord: {}, {}", topLeftWorldCoord.x, topLeftWorldCoord.y);
		LOG_DEBUG("offset: {}, {}", offset.x, offset.y);
		LOG_DEBUG("offset / unitGridSize: {}, {}", gridFloat.x, gridFloat.y);
		LOG_DEBUG("regionIndex: {}, {}", regionIndex.x, regionIndex.y);

		if (regionIndex.x < 0 || regionIndex.x >= static_cast<int>(regionGridCount.x) ||
			regionIndex.y < 0 || regionIndex.y >= static_cast<int>(regionGridCount.y))
		{
			std::cout << "⚠️ FindIndexWhichGridObjectIsInside returned false (out of bounds)\n";
			return {-1, -1};
		}

		std::cout << "✅ Index Valid! Returning: " << regionIndex.x << ", " << regionIndex.y << "\n";
		return regionIndex;
	}

}