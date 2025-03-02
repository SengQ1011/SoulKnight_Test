//
// Created by QuzzS on 2025/3/2.
//

#ifndef CURSOR_HPP
#define CURSOR_HPP

#include "pch.hpp"

class Cursor
{
public:
	[[deprecated("SetCursorToCenter功能無法實現")]]
	static void SetCursorToCenter();

	static void SetWindowOriginWorldCoord(const glm::vec2& windowOriginWorldCoord);

	static void ResetWindowOriginWorldCoord();

	static glm::vec2 GetCursorWindowCoord();

	static glm::vec2 GetCursorWorldCoord();

protected:
	inline static glm::vec2 m_WindowCoord = {0, 0};
	inline static glm::vec2 m_WindowOriginWorldCoord = {0,0};
};

#endif //CURSOR_HPP
