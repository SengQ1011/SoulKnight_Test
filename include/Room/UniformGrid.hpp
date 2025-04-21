//
// Created by QuzzS on 2025/4/20.
//

#ifndef UNIFORMGRID_HPP
#define UNIFORMGRID_HPP

#include <memory>
#include <vector>
#include <unordered_map>

class nGameObject;
struct Rect;

class UniformGrid {
public:
	void Initialize(float worldWidth, float worldHeight, float cellSize);

	void Clear();
	void Insert(const std::shared_ptr<nGameObject>& obj, const Rect& aabb);
	std::vector<std::shared_ptr<nGameObject>> QueryNearby(const Rect& aabb) const;

private:
	int m_NumCols = 35, m_NumRows = 35;
	float m_CellSize = 16;
	std::unordered_map<int, std::vector<std::shared_ptr<nGameObject>>> m_Cells;

	int GetCellIndex(int col, int row) const;
	std::vector<int> GetOverlappingCells(const Rect& aabb) const;
};

#endif //UNIFORMGRID_HPP
