//
// Created by QuzzS on 2025/3/2.
//

#include "Cursor.hpp"
#include "config.hpp"
#include "Util/Input.hpp"

void Cursor::SetCursorToCenter()
{
	// SDL_WarpMouseGlobal(0,0);
	// SDL_WarpMouseInWindow(nContext::GetSDLWindow(),WINDOW_WIDTH/2,WINDOW_HEIGHT/2);
}

void Cursor::SetWindowOriginWorldCoord(const glm::vec2& windowOriginWorldCoord)
{
	m_WindowOriginWorldCoord = windowOriginWorldCoord;
}

void Cursor::ResetWindowOriginWorldCoord()
{
	m_WindowOriginWorldCoord = glm::vec2(0.0f);
}

glm::vec2 Cursor::GetCursorWindowCoord()
{
	int x, y;
	SDL_GetMouseState(&x,&y);

	//SDL_coord => OpenGL_coord
	m_WindowCoord = glm::vec2(x,y) - glm::vec2(PTSD_Config::WINDOW_WIDTH/2,PTSD_Config::WINDOW_HEIGHT/2);
	m_WindowCoord.y *= -1;
	return m_WindowCoord;
}

glm::vec2 Cursor::GetCursorWorldCoord(const float scale)
{
	return GetCursorWindowCoord() / scale + m_WindowOriginWorldCoord;
}


