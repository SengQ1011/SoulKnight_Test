//
// Created by QuzzS on 2025/4/20.
//

#ifndef MINIMAPPANEL_HPP
#define MINIMAPPANEL_HPP

#include <array>
#include <memory>
#include <vector>
#include "UIPanel.hpp"
#include "glm/glm.hpp"


class DungeonMap;
class DungeonRoom;
class nGameObject;
enum class RoomState;
enum class RoomType;
struct RoomInfo;

class MinimapPanel : public UIPanel
{
public:
	explicit MinimapPanel(const std::shared_ptr<DungeonMap> &dungeonMap);
	~MinimapPanel() override = default;

	void Start() override;
	void Update() override;

	// Override Show/Hide for proper setup
	void Show() override;
	void Hide() override;

private:
	static const int MINIMAP_SIZE = 128;
	static const int BOX_ICON_SIZE = 16;
	static const int MAP_GRID_SIZE = 5;
	static const int ROAD_HORIZONTAL_WIDTH = 8;
	static const int ROAD_HORIZONTAL_HEIGHT = 4;
	static const int ROAD_VERTICAL_WIDTH = 4;
	static const int ROAD_VERTICAL_HEIGHT = 8;

	// 核心引用
	std::weak_ptr<DungeonMap> m_DungeonMap;
	std::weak_ptr<DungeonRoom> m_LastCurrentRoom; // 用於檢測房間變化

	// UI元素
	std::shared_ptr<nGameObject> m_MinimapBackground; // 128x128背景

	// 房間圖標陣列 (5x5)
	std::array<std::shared_ptr<nGameObject>, 25> m_RoomBoxIcons;
	std::array<std::shared_ptr<nGameObject>, 25> m_RoomIcons;

	// 通道圖標池
	std::vector<std::shared_ptr<nGameObject>> m_HorizontalRoads; // road_0 (8x4)
	std::vector<std::shared_ptr<nGameObject>> m_VerticalRoads; // road_1 (4x8)

	// 位置控制
	glm::vec2 m_MinimapCenter = glm::vec2(530.0f, 200.0f); // 小地圖在螢幕上的位置
	glm::ivec2 m_StartingRoomGridPos = glm::ivec2(2, 2); // 初始房間在5x5格子中的位置
	glm::vec2 m_StartingRoomOffset = glm::vec2(0, 0); // Starting Room的偏移量

	// Debug 相關成員變量
	bool m_DebugInitialized = false;
	bool m_PosChangedByInput = false;
	glm::vec2 m_DebugPos = glm::vec2(0.0f);
	bool m_EnableDebug = false; // 控制是否顯示 Debug 視窗

	// 初始化方法
	void InitializeMinimapElements();
	void InitializeRoomIcons();
	void InitializeRoadPools();

	// 更新方法
	void CheckCurrentRoomChanged();
	void UpdateMinimapLayout();
	void UpdateRoomVisibility();
	void UpdateRoadVisibility();

	// Debug 方法
	void DrawDebugUI();
	void UpdateAllElementsPosition();

	// 座標轉換
	glm::vec2 GridToMinimapPos(const glm::ivec2 &gridPos) const;
	bool IsInMinimapBounds(const glm::vec2 &minimapPos) const;

	// 房間狀態更新
	void UpdateRoomIcon(int roomIndex, RoomState state, RoomType type, bool isCurrentRoom = false);
	void SetRoomVisible(int roomIndex, bool visible);

	// 可見性判斷
	bool ShouldShowRoom(const RoomInfo &roomInfo, const glm::ivec2 &gridPos);
	bool HasExploredNeighbor(const glm::ivec2 &gridPos);

	// 通道管理
	void UpdateHorizontalRoad(const glm::ivec2 &fromGrid, const glm::ivec2 &toGrid, bool visible);
	void UpdateVerticalRoad(const glm::ivec2 &fromGrid, const glm::ivec2 &toGrid, bool visible);
	std::shared_ptr<nGameObject> GetAvailableHorizontalRoad();
	std::shared_ptr<nGameObject> GetAvailableVerticalRoad();

	// 資源路徑輔助
	std::string GetRoomIconPath(RoomType type) const;
};

#endif // MINIMAPPANEL_HPP
