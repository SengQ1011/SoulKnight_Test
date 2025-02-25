//
// Created by QuzzS on 2025/2/25.
//

#ifndef TOOL_HPP
#define TOOL_HPP
#include <SDL_mouse.h>
#include <glm/vec2.hpp>

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

}

#endif //TOOL_HPP
