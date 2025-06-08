//
// Created by Assistant on 2025/1/20.
//

#ifndef WALLOBJECT_HPP
#define WALLOBJECT_HPP

#include <glm/vec2.hpp>
#include "Override/nGameObject.hpp"


// 牆壁物件類別 - 用於碰撞優化
class WallObject : public nGameObject
{
public:
	explicit WallObject(const std::string &baseName = "wall") : nGameObject(baseName, "WallObject") {}

	~WallObject() override = default;

	// 標識這是牆壁物件
	bool IsWall() const { return true; }

	// 重寫類別名稱
	std::string GetClassName() const override { return "WallObject"; }

	// 獲取網格位置（用於碰撞優化）
	glm::ivec2 GetGridPosition() const { return m_GridPosition; }
	void SetGridPosition(const glm::ivec2 &gridPos) { m_GridPosition = gridPos; }

private:
	glm::ivec2 m_GridPosition{0, 0}; // 在房間網格中的位置
};

#endif // WALLOBJECT_HPP
