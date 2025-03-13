//
// Created by tjx20 on 3/7/2025.
//

#ifndef COLLISIONBOX_HPP
#define COLLISIONBOX_HPP

#include "Override/nGameObject.hpp"

class CollisionBox {
public:
	CollisionBox(float width, float height, float offsetX, float offsetY);
	~CollisionBox() = default;

	bool CheckCollision(const CollisionBox& other) const;

	float GetX(float characterX) const { return characterX + m_offsetX; }
	float GetY(float characterY) const { return characterY + m_offsetY; }
	float GetWidth() const { return m_width; }
	float GetHeight() const { return m_height; }

	void SetCoordinate(glm::vec2 coordinate);

private:
	float m_width, m_height;
	float m_offsetX, m_offsetY; // 碰撞箱相對於角色位置的偏移量
};

#endif //COLLISIONBOX_HPP