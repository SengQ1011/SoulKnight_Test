//
// Created by QuzzS on 2025/4/27.
//
#include "Structs/CollisionComponentStruct.hpp"
#include "glm/detail/func_geometric.inl" //

void CollisionInfo::SetCollisionNormal(const glm::vec2 &normal)
{
	if (glm::length(normal) > 0.0f)
	{
		collisionNormal = glm::normalize(normal);
	}
	else
	{
		collisionNormal = glm::vec2(1.0f, 0.0f);
	}
}

glm::vec2 CollisionInfo::GetCollisionNormal() const
{
	// 使用点积计算长度的平方，避免开方
	if (glm::dot(collisionNormal, collisionNormal) > std::numeric_limits<float>::epsilon())
	{
		return glm::normalize(collisionNormal);
	}
	return glm::vec2(0.0f); // 明确返回零向量 TODO:零向量要怎麽解決？
}

void CollisionEventInfo::SetCollisionNormal(const glm::vec2 &normal)
{
	if (glm::length(normal) > 0.0f)
	{
		collisionNormal = glm::normalize(normal);
	}
	else
	{
		collisionNormal = glm::vec2(1.0f, 0.0f);
	}
}

glm::vec2 CollisionEventInfo::GetCollisionNormal() const
{
	// 使用点积计算长度的平方，避免开方
	if (glm::dot(collisionNormal, collisionNormal) > std::numeric_limits<float>::epsilon())
	{
		return glm::normalize(collisionNormal);
	}
	return glm::vec2(0.0f); // 明确返回零向量 TODO:零向量要怎麽解決？
}

bool Rect::Intersects(const Rect &other) const
{
	constexpr float epsilon = 0.01f; // 緩衝邊界 咬到太緊不好喔
	return !(right() < other.left() + epsilon || left() > other.right() - epsilon ||
			 bottom() > other.top() - epsilon || top() < other.bottom() + epsilon);
}
