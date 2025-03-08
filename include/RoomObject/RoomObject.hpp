//
// Created by QuzzS on 2025/3/8.
//

#ifndef ROOMOBJECT_HPP
#define ROOMOBJECT_HPP

#include "Override/nGameObject.hpp"
#include "pch.hpp"

class RoomObject : public nGameObject{
public:
	~RoomObject() override = default;
	glm::vec2 GetTileHitBox();
protected:
	glm::vec2 tileHitBox = {0,0};
};

#endif //ROOMOBJECT_HPP
