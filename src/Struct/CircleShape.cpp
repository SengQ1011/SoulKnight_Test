//
// Created by QuzzS on 2025/5/2.
//

#include "Structs/CircleShape.hpp"
#include "Structs/RectShape.hpp"
#include <glm/glm.hpp>

std::shared_ptr<Util::Image> CircleShape::s_RedAreaImage = nullptr;
std::shared_ptr<Util::Image> CircleShape::s_YellowAreaImage = nullptr;

// CircleShape::CircleShape(const glm::vec2 &center, float radius)
// 	: m_Center(center), m_Radius(radius)
// {
//
// }


bool CircleShape::Intersects(const std::shared_ptr<AreaShape>& other) const {
	return other->IntersectsWithCircle(*this);
}

bool CircleShape::IntersectsWithCircle(const CircleShape& other) const {
	const float r = m_Radius + other.m_Radius;
	return glm::dot(m_Center, other.m_Center) <= r * r;
}

bool CircleShape::IntersectsWithRect(const RectShape& other) const {
	// 反向調用 Rect 的 IntersectWithCircle
	return other.IntersectsWithCircle(*this);
}

bool CircleShape::ContainsPoint(const glm::vec2 &other) const
{
	return glm::length(other - m_Center) <= m_Radius;
}
