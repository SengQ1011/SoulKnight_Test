//
// Created by QuzzS on 2025/2/25.
//

#ifndef TOOL_HPP
#define TOOL_HPP

#include "glm/vec2.hpp"

namespace Tool
{
	struct Rectangle
	{
		glm::vec2 v_Min; //右上
		glm::vec2 v_Max; //左下
	};


	/**
	 * @brief To draw a HitArea or bound物件邊界/範圍
	 * @param center Object's centroid 物件中心
	 * @param size 物件大小
	 * @return A Rectangle containing the range of object物件所占範圍
	 */
	Rectangle Bound(glm::vec2 center, glm::vec2 size);

	/**
	 * @brief Converts SDL coordinate system to GLM coordinate system.
	 * @param x The x-coordinate in SDL coordinate system.
	 * @param y The y-coordinate in SDL coordinate system.
	 * @return A glm::vec2 containing the coordinates in GLM coordinate system. {x,y}
	 */
	glm::vec2 Sdl2Glm(int x, int y);

	/**
	 * @brief Get cursor position in GLM coordinate system.
	 * @return vec2(x,y) representing the cursor coordinates.
	 */
	glm::vec2 GetMouseCoord();

	glm::ivec2 WorldToMapGrid(const glm::vec2& worldPos, const glm::vec2& tileSize, const glm::vec2& roomRegion, const glm::ivec2& roomNum);
	glm::vec2 MapGridToWorld(const glm::ivec2& gridPos, const glm::vec2& tileSize, const glm::vec2& roomRegion, const glm::ivec2& roomNum);
	glm::ivec2 WorldToRoomGrid(const glm::vec2& worldPos, const glm::vec2& tileSize, const glm::vec2& roomCoord, const glm::vec2& roomRegion);
	glm::vec2 RoomGridToWorld(const glm::ivec2& grid, const glm::vec2& tileSize, const glm::vec2& roomCoord, const glm::vec2& roomRegion);


	/**
	 * @ brief 計算一個物件的世界座標對應到哪個「區域格子」索引（例如對應哪個房間或區塊）。
	 *
	 * 此函數假設整個地圖是由多個等大小的區域（如房間）組成，且每個區域包含固定格數，
	 * 根據物件的世界座標與地圖的格子組成與尺寸，計算其落在哪個區域中。
	 *
	 * @param regionGridCount 區域中网格（如地图有的房間或房间有的格子）的个數，例如Room有35x35格子, Map有5x5房间区域。
	 * @param unitGridSize 每個网格的像素大小，例如Room的格子有16x16像素, Map的房间区域有(35x16)x(35x16)像素。
	 * @param objectWorldCoord 物件的世界座標（像素單位）。
	 * @param regionCenterPointWorldCoord 区域中心点的世界坐标
	 * @return glm::vec2 物件所在的區域索引（如房間索引），第一個為 x（列），第二個為 y（行）。
	 *         若座標越界，可能會回傳負值或超出 regionGridCount 範圍的索引。
	 */
	glm::ivec2 FindIndexWhichGridObjectIsInside(
		const glm::vec2& regionGridCount,
		const glm::vec2& unitGridSize,
		const glm::vec2& objectWorldCoord,
		const glm::vec2& regionCenterPointWorldCoord = glm::vec2(0));
}

#endif //TOOL_HPP
