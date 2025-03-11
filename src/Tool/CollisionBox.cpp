//
// Created by tjx20 on 3/11/2025.
//
#include "Tool/CollisionBox.hpp"

CollisionBox::CollisionBox(float width, float height, float offsetX, float offsetY):
	m_width(width), m_height(height), m_offsetX(offsetX), m_offsetY(offsetY){};

bool CollisionBox::CheckCollision(const CollisionBox &other) const {
		return !(m_offsetX + m_width < other.m_offsetX || other.m_offsetX + other.m_width < m_offsetX ||
				 m_offsetY + m_height < other.m_offsetY || other.m_offsetY + other.m_height < m_offsetY);
}

