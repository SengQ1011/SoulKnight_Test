//
// Created by QuzzS on 2025/5/2.
//
#include "Structs/RectShape.hpp"
#include "Structs/CircleShape.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include <algorithm>

// 計算頂點（逆時針）
std::vector<glm::vec2> RectShape::GetVertices() const {
	std::vector<glm::vec2> vertices(4);
	glm::vec2 right = glm::vec2(std::cos(m_RotationRad), std::sin(m_RotationRad));
	glm::vec2 up = glm::vec2(-right.y, right.x);

	vertices[0] = m_Center + (-m_HalfSize.x * right - m_HalfSize.y * up); // 左下
	vertices[1] = m_Center + ( m_HalfSize.x * right - m_HalfSize.y * up); // 右下
	vertices[2] = m_Center + ( m_HalfSize.x * right + m_HalfSize.y * up); // 右上
	vertices[3] = m_Center + (-m_HalfSize.x * right + m_HalfSize.y * up); // 左上
	return vertices;
}

RectShape RectShape::GetBoundingAABB() const {
	auto verts = GetVertices();
	glm::vec2 min = verts[0], max = verts[0];
	for (const auto& v : verts) {
		min = glm::min(min, v);
		max = glm::max(max, v);
	}
	return RectShape((min + max) * 0.5f, (max - min) * 0.5f, 0.0f);
}

bool RectShape::Intersects(const std::shared_ptr<AreaShape>& other) const {
	return other->IntersectsWithRect(*this);
}

bool RectShape::IntersectsWithRect(const RectShape& other) const {
	// AABB 快速檢測
	if (m_RotationRad == 0 && other.m_RotationRad == 0) {
		glm::vec2 minA = m_Center - m_HalfSize;
		glm::vec2 maxA = m_Center + m_HalfSize;
		glm::vec2 minB = other.m_Center - other.m_HalfSize;
		glm::vec2 maxB = other.m_Center + other.m_HalfSize;

		if (maxA.x < minB.x || maxB.x < minA.x || maxA.y < minB.y || maxB.y < minA.y)
			return false;
		return true;
	}

	// SAT 精確檢查
	auto vertsA = GetVertices();
	auto vertsB = other.GetVertices();

	auto getAxes = [](const std::vector<glm::vec2>& verts) {
		std::vector<glm::vec2> axes;
		for (size_t i = 0; i < verts.size(); ++i) {
			glm::vec2 edge = verts[(i + 1) % verts.size()] - verts[i];
			glm::vec2 normal = glm::normalize(glm::vec2(-edge.y, edge.x));
			axes.push_back(normal);
		}
		return axes;
	};

	auto project = [](const std::vector<glm::vec2>& verts, const glm::vec2& axis, float& min, float& max) {
		min = max = glm::dot(verts[0], axis);
		for (const auto& v : verts) {
			float p = glm::dot(v, axis);
			min = std::min(min, p);
			max = std::max(max, p);
		}
	};

	auto axes = getAxes(vertsA);
	auto axesB = getAxes(vertsB);
	axes.insert(axes.end(), axesB.begin(), axesB.end());

	for (const auto& axis : axes) {
		float minA, maxA, minB, maxB;
		project(vertsA, axis, minA, maxA);
		project(vertsB, axis, minB, maxB);
		if (maxA < minB || maxB < minA)
			return false;
	}
	return true;
}

bool RectShape::IntersectsWithCircle(const CircleShape& other) const {
	// SAT with special axis
	auto verts = GetVertices();

	// Step 1: find the closest point
	glm::vec2 closest = verts[0];
	float minDistSq = dot(other.GetCenter(), closest);
	for (const auto& v : verts) {
		float distSq = dot(other.GetCenter(), v);
		if (distSq < minDistSq) {
			minDistSq = distSq;
			closest = v;
		}
	}
	glm::vec2 axis = glm::normalize(other.GetCenter() - closest);

	// Step 2: add normal axes
	auto getAxes = [](const std::vector<glm::vec2>& verts) {
		std::vector<glm::vec2> axes;
		for (size_t i = 0; i < verts.size(); ++i) {
			glm::vec2 edge = verts[(i + 1) % verts.size()] - verts[i];
			glm::vec2 normal = glm::normalize(glm::vec2(-edge.y, edge.x));
			axes.push_back(normal);
		}
		return axes;
	};

	auto project = [](const std::vector<glm::vec2>& verts, const glm::vec2& axis, float& min, float& max) {
		min = max = dot(verts[0], axis);
		for (const auto& v : verts) {
			float p = dot(v, axis);
			min = std::min(min, p);
			max = std::max(max, p);
		}
	};

	auto axes = getAxes(verts);
	axes.push_back(axis); // special axis for circle

	for (const auto& ax : axes) {
		float minR, maxR, minC, maxC;
		project(verts, ax, minR, maxR);

		float centerProj = dot(other.GetCenter(), ax);
		minC = centerProj - other.GetRadius();
		maxC = centerProj + other.GetRadius();

		if (maxR < minC || maxC < minR)
			return false;
	}
	return true;
}

bool RectShape::ContainsPoint(const glm::vec2 &other) const
{
	glm::vec2 dir = other - m_Center;
	glm::vec2 unrotated = glm::rotate(dir, -m_RotationRad);

	return std::abs(unrotated.x) <= m_HalfSize.x && std::abs(unrotated.y) <= m_HalfSize.y;
}
