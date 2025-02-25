//
// Created by QuzzS on 2025/2/25.
//

#include "Tool.hpp"

#include "config.hpp"

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
}