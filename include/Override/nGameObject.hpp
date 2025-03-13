//
// Created by QuzzS on 2025/3/1.
//

#ifndef NGAMEOBJECT_HPP
#define NGAMEOBJECT_HPP

#include "Util/GameObject.hpp"
#include "Util/Logger.hpp"

class nGameObject : public Util::GameObject
{
public:
	nGameObject() = default;

	glm::vec2 m_WorldCoord = {0,0};

	[[nodiscard]] glm::vec2 GetPivot() const { return m_Pivot;}

	[[nodiscard]] glm::vec2 GetWorldCoord() const {return m_WorldCoord;}

};

#endif //NGAMEOBJECT_HPP
