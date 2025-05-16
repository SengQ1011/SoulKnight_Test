//
// Created by QuzzS on 2025/5/2.
//

#ifndef IAREASHAPE_HPP
#define IAREASHAPE_HPP

#include <memory>
#include <glm/vec2.hpp>

/**
 * @brief 可用於碰撞偵測的形狀類型。
 */
enum class ShapeType {
	Null,	///< 沒有形狀（沒有碰撞範圍）
	Rect,	///< 矩形（含可旋轉）
	Circle  ///< 圓形
};

class RectShape;
class CircleShape;
namespace Util { class Image; }

/**
 * @brief 所有形狀的基底類別，定義基本碰撞接口。
 *
 * 使用雙分派（double dispatch）模式進行類型正確的碰撞檢測。
 */
/*
 * shapeA->Intersects(shapeB)
 * shapeA知道自己的ShapeType,所以在Intersects裏面呼叫shapeB對應的IntersectsWithShape函式
 * 假設shapeA是Circle
 * 那shapeA->Intersects内容就是
 * other->IntersetsWithCircle(*this); other是shapeB this是shapeA
 * 這就是雙分派
 */
class AreaShape {
public:
	virtual ~AreaShape() = default;

	/**
	 * @brief 取得形狀的類型（Rect 或 Circle）。
	 * @return ShapeType 列舉值。
	 */
	virtual ShapeType GetType() const = 0;

	virtual void SetCenter(const glm::vec2& center) = 0;

	/**
	 * @brief 主碰撞接口，與其他形狀進行碰撞檢查。
	 * @param other 另一個形狀（可為圓或矩形）。
	 * @return 是否發生碰撞。
	 */
	virtual bool Intersects(const std::shared_ptr<AreaShape>& other) const = 0;

	// 雙分派分流
	/**
	 * @brief 與矩形的具體碰撞邏輯。
	 * @param other 矩形物件。
	 * @return 是否碰撞。
	 */
	virtual bool IntersectsWithRect(const RectShape& other) const = 0;
	/**
	 * @brief 與圓形的具體碰撞邏輯。
	 * @param other 圓形物件。
	 * @return 是否碰撞。
	 */
	virtual bool IntersectsWithCircle(const CircleShape& other) const = 0;
	/**
	 * @brief 點是否包含在範圍裏面。
	 * @param other 點。
	 * @return 是否包含。
	 */
	virtual bool ContainsPoint(const glm::vec2& other) const = 0;

};
#endif //IAREASHAPE_HPP
