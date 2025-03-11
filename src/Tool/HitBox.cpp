//
// Created by tjx20 on 3/11/2025.
//

#include "Tool/HItBox.hpp"

HitBox::HitBox(float x, float y, float width, float height){};

bool HitBox::CheckCollision(const HitBox &other) const {
	return !(x + width < other.x || other.x + other.width < x ||
			 y + height < other.y || other.y + other.height < y);
}