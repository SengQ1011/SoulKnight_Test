//
// Created by tjx20 on 3/11/2025.
//
#include "Tool/CollisionBox.hpp"

CollisionBox::CollisionBox(float x, float y, float width, float height){};

bool CollisionBox::CheckCollision(const CollisionBox &other) const {
		return !(x + width < other.x || other.x + other.width < x ||
				 y + height < other.y || other.y + other.height < y);
}

