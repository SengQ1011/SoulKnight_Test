//
// Created by QuzzS on 2025/5/2.
//

#ifndef CIRCLESHAPE_HPP
#define CIRCLESHAPE_HPP

#include <memory>
#include "AreaShape.hpp"
#include "glm/vec2.hpp"

namespace Util
{
	class Image;
}
class RectShape; // 前向宣告

/**
 * @brief 表示一個圓形形狀，用於範圍檢測。
 *
 * 儲存圓心座標與半徑，並支援與其他形狀（圓形、矩形）的碰撞判斷。
 */
class CircleShape : public AreaShape {
public:
	/**
	 * @brief 建立一個圓形。
	 * @param center 圓心座標。
	 * @param radius 半徑。
	 */
	CircleShape(const glm::vec2& center, float radius)
		: m_Center(center), m_Radius(radius) {}

	/**
	 * @brief 取得形狀類型。
	 * @return ShapeType::Circle。
	 */
	ShapeType GetType() const override { return ShapeType::Circle; }

	void SetCenter(const glm::vec2 &center) override { m_Center = center; }

	/**
	 * @brief 取得圓心位置。
	 * @return 圓心的 glm::vec2坐標。
	 */
	const glm::vec2& GetCenter() const { return m_Center; }

	/**
	 * @brief 取得半徑。
	 * @return 半徑的 float 值。
	 */
	float GetRadius() const { return m_Radius; }

	bool Intersects(const std::shared_ptr<AreaShape>& other) const override;

	bool IntersectsWithRect(const RectShape& other) const override;
	bool IntersectsWithCircle(const CircleShape& other) const override;
	bool ContainsPoint(const glm::vec2 &other) const override;

private:
	glm::vec2 m_Center; ///<圓心位置
	float m_Radius;		///<圓的半徑

	//顯示共用
	static std::shared_ptr<Util::Image> s_RedAreaImage;
	static std::shared_ptr<Util::Image> s_YellowAreaImage;
};

#endif //CIRCLESHAPE_HPP
