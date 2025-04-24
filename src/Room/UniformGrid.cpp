//
// Created by QuzzS on 2025/4/20.
//

#include "Room/UniformGrid.hpp"
#include <unordered_set>
#include "Structs/CollisionComponentStruct.hpp"

void UniformGrid::Initialize(float worldWidth, float worldHeight, float cellSize) {
	m_CellSize = cellSize;
	m_NumCols = static_cast<int>(std::ceil(worldWidth / cellSize));
	m_NumRows = static_cast<int>(std::ceil(worldHeight  / cellSize));
	m_Cells.clear();
}

void UniformGrid::Clear() {
	m_Cells.clear();
}

int UniformGrid::GetCellIndex(int col, int row) const {
	return row * m_NumCols + col;
}

std::vector<int> UniformGrid::GetOverlappingCells(const Rect& aabb) const {
	std::vector<int> result;
	int left = static_cast<int>(aabb.left() / m_CellSize);
	int right = static_cast<int>(aabb.right() / m_CellSize);
	int top = static_cast<int>(aabb.top() / m_CellSize);
	int bottom = static_cast<int>(aabb.bottom() / m_CellSize);

	for (int row = bottom; row <= top; ++row) {
		for (int col = left; col <= right; ++col) {
			result.push_back(GetCellIndex(col, row));
		}
	}
	return result;
}

void UniformGrid::Insert(const std::shared_ptr<nGameObject>& obj, const Rect& aabb) {
	auto cells = GetOverlappingCells(aabb);
	for (int index : cells) {
		m_Cells[index].push_back(obj);
	}
}

std::vector<std::shared_ptr<nGameObject>> UniformGrid::QueryNearby(const Rect& aabb) const {
	std::vector<std::shared_ptr<nGameObject>> result;
	std::unordered_set<std::shared_ptr<nGameObject>> uniqueSet;

	for (int index : GetOverlappingCells(aabb)) {
		auto it = m_Cells.find(index);
		if (it != m_Cells.end()) {
			for (const auto& obj : it->second) {
				uniqueSet.insert(obj);
			}
		}
	}

	result.assign(uniqueSet.begin(), uniqueSet.end());
	return result;
}