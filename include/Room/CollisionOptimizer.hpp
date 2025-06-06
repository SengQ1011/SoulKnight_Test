//
// Created by Assistant on 2025/1/20.
//

#ifndef COLLISIONOPTIMIZER_HPP
#define COLLISIONOPTIMIZER_HPP

#include <glm/vec2.hpp>
#include <memory>
#include <vector>


// 前向聲明
class nGameObject;
class WallObject;
struct Rect;

// 碰撞區域結構
struct CollisionRect
{
	glm::ivec2 topLeft; // 左上角網格座標
	glm::ivec2 bottomRight; // 右下角網格座標
	glm::vec2 worldPos; // 世界座標位置
	glm::vec2 size; // 碰撞箱大小
};

// 碰撞優化管理器
class CollisionOptimizer
{
public:
	CollisionOptimizer() = default;
	~CollisionOptimizer() = default;

	// 分析房間物件並生成優化的碰撞箱
	std::vector<CollisionRect> OptimizeWallCollisions(const std::vector<std::shared_ptr<nGameObject>> &roomObjects,
													  const glm::vec2 &tileSize, const glm::vec2 &roomWorldCoord,
													  int gridSize = 35);

private:
	// 檢查物件是否為牆壁
	bool IsWallObject(const std::shared_ptr<nGameObject> &obj) const;

	// 從牆壁物件建立網格
	std::vector<std::vector<bool>> CreateWallGrid(const std::vector<std::shared_ptr<nGameObject>> &roomObjects,
												  int gridSize, const glm::vec2 &tileSize,
												  const glm::vec2 &roomWorldCoord);

	// 尋找連續的矩形區域
	std::vector<CollisionRect> FindRectangularRegions(const std::vector<std::vector<bool>> &wallGrid,
													  const glm::vec2 &tileSize, const glm::vec2 &roomWorldCoord);

	// 將網格座標轉換為世界座標
	glm::vec2 GridToWorldCoord(const glm::ivec2 &gridPos, const glm::vec2 &tileSize,
							   const glm::vec2 &roomWorldCoord) const;

	// 計算兩個矩形的交集面積
	float CalculateIntersectionArea(const Rect &a, const Rect &b) const;
};

#endif // COLLISIONOPTIMIZER_HPP
