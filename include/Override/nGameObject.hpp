//
// Created by QuzzS on 2025/3/1.
//

#ifndef NGAMEOBJECT_HPP
#define NGAMEOBJECT_HPP

#include "Util/GameObject.hpp"

class nGameObject : public Util::GameObject
{
public:
	glm::vec2 m_Offset = {0, 0}; //相對於camera的位置
	glm::vec2 m_WorldCoord = {0,0};

	[[nodiscard]] glm::vec2 GetPivot() const { return m_Pivot;}

	[[nodiscard]] glm::vec2 GetWorldCoord() const {return m_WorldCoord;}

	[[nodiscard]] glm::vec2 GetOffset() const {return m_Offset;}
};

#endif //NGAMEOBJECT_HPP
