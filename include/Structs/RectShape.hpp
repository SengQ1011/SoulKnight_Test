//
// Created by QuzzS on 2025/5/2.
//

#ifndef RECTSHAPE_HPP
#define RECTSHAPE_HPP

#include "AreaShape.hpp"
#include "glm/vec2.hpp"
#include <vector>

class CircleShape; // 前向宣告

/**
 * @brief 表示一個可旋轉的矩形，用於範圍檢測。
 *
 * 儲存中心點、半長寬與旋轉角度，支援與其他形狀的碰撞判斷。
 * 當旋轉為 0 時，可視為 AABB（軸對齊矩形）並支援快速判斷。
 */
class RectShape : public AreaShape {
public:
	/**
	 * @brief 建立一個矩形。
	 * @param center 中心位置。
	 * @param halfSize 半長寬（中心到邊界的距離）。
	 * @param rotation 旋轉角度（弧度），預設為 0。
	 */
	RectShape(const glm::vec2& center, const glm::vec2& halfSize, float rotation = 0.0f)
		: m_Center(center), m_HalfSize(halfSize), m_RotationRad(rotation) {}

	/**
	 * @brief 取得形狀類型。
	 * @return ShapeType::Rect。
	 */
	ShapeType GetType() const override { return ShapeType::Rect; }

	void SetCenter(const glm::vec2 &center) override { m_Center = center; };

	/**
	 * @brief 取得中心位置。
	 * @return 中心座標。
	 */
	const glm::vec2& GetCenter() const { return m_Center; }

	/**
	 * @brief 取得半長寬。
	 * @return 各軸的半長寬向量。
	 */
	const glm::vec2& GetHalfSize() const { return m_HalfSize; }

	/**
	 * @brief 取得旋轉角度（弧度）。
	 * @return 弧度制旋轉值。
	 */
	float GetRotation() const { return m_RotationRad; }

	/**
	 * @brief 取得矩形的 4 個頂點（考慮旋轉），逆時針順序。（左下→右下→右上→左上）
	 * @return 頂點陣列。
	 */
	std::vector<glm::vec2> GetVertices() const;

	/**
	 * @brief 取得包含矩形的最小 AABB。
	 * @return 軸對齊矩形。
	 */
	RectShape GetBoundingAABB() const;

	bool Intersects(const std::shared_ptr<AreaShape>& other) const override;

	bool IntersectsWithRect(const RectShape& other) const override;
	bool IntersectsWithCircle(const CircleShape& other) const override;
	bool ContainsPoint(const glm::vec2 &other) const override;

private:
	glm::vec2 m_Center;
	glm::vec2 m_HalfSize;
	float m_RotationRad; // 弧度
};

#endif //RECTSHAPE_HPP
