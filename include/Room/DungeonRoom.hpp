//
// Created by QuzzS on 2025/4/9.
//

#ifndef DUNGEONROOM_HPP
#define DUNGEONROOM_HPP

#include <array>
#include <memory>
#include "Room.hpp"

class CollisionComponent;
struct Rect;

// 前向聲明
class DungeonRoom;
class GridSystem;
class RoomConnectionManager;
class TerrainGenerator;
struct CollisionRect;

// 常量定義
namespace RoomConstants
{
	constexpr int GRID_SIZE = 35;
	constexpr float CORRIDOR_WIDTH = 7.0f;
	constexpr float INTERSECTION_THRESHOLD = 0.5f;
} // namespace RoomConstants

// 房間狀態枚舉
enum class RoomState
{
	UNEXPLORED, // 未探索
	EXPLORED, // 已探索
	COMBAT, // 戰鬥中
	CLEANED, // 已清除
};

// 方向枚舉
enum class Direction
{
	UP = 0,
	RIGHT = 1,
	DOWN = 2,
	LEFT = 3
};

// 房間類型枚舉
enum class RoomType
{
	EMPTY,
	DUNGEON,
	STARTING,
	PORTAL,
	MONSTER,
	BOSS,
	CHEST,
	SPECIAL,
};

// 房間連接管理器（分離出來的類）
class RoomConnectionManager
{
public:
	void SetNeighbor(Direction dir, const std::weak_ptr<DungeonRoom> &room);
	void SetConnection(Direction dir, bool connected);

	std::weak_ptr<DungeonRoom> GetNeighbor(Direction dir) const;
	bool HasConnection(Direction dir) const;

	std::vector<std::weak_ptr<DungeonRoom>> GetConnectedNeighbors() const;
	std::vector<Direction> GetConnectedDirections() const;

private:
	std::array<std::weak_ptr<DungeonRoom>, 4> m_Neighbors;
	std::array<bool, 4> m_HasConnection = {false, false, false, false};
};

// 網格系統管理器
class GridSystem
{
public:
	GridSystem();

	void Initialize();
	void MarkPosition(int row, int col, int value = 1);
	bool IsPositionBlocked(int row, int col) const;

	const std::vector<std::vector<int>> &GetGrid() const { return m_Grid; }

	// 碰撞檢測相關
	void UpdateGridFromObjects(const std::vector<std::shared_ptr<nGameObject>> &objects,
							   const RoomSpaceInfo &spaceInfo);
	float CalculateIntersectionArea(const Rect &a, const Rect &b) const;

private:
	std::vector<std::vector<int>> m_Grid;

	bool IsValidPosition(int row, int col) const;
};

// 地形生成器
class TerrainGenerator
{
public:
	explicit TerrainGenerator(std::weak_ptr<RoomObjectFactory> factory);

	std::shared_ptr<nGameObject> CreateWall(int row, int col, const RoomSpaceInfo &spaceInfo);
	std::shared_ptr<nGameObject> CreateFloor(int row, int col, const RoomSpaceInfo &spaceInfo);
	std::shared_ptr<nGameObject> CreateDoor(int row, int col, const RoomSpaceInfo &spaceInfo);

private:
	std::weak_ptr<RoomObjectFactory> m_Factory;
};

class DungeonRoom : public Room
{
public:
	explicit DungeonRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader> &loader,
						 const std::shared_ptr<RoomObjectFactory> &room_object_factory,
						 const glm::vec2 &mapGridPos = glm::vec2(0, 0), const RoomType roomType = RoomType::DUNGEON);

	~DungeonRoom() override = default;

	// 基類方法重寫
	void Start(const std::shared_ptr<Character> &player) override;
	void Update() override;
	void LoadFromJSON() override;

	// 房間狀態管理
	virtual void TryActivateByPlayer() { SetState(RoomState::EXPLORED); }
	virtual void OnStateChanged() {} // 子類可重寫

	void SetState(RoomState newState);
	RoomState GetState() const { return m_State; }

	// 基本屬性訪問
	RoomType GetRoomType() const { return m_RoomType; }
	glm::vec2 GetMapGridPos() const { return m_MapGridPos; }

	// 玩家位置檢測（擴展父類別的基礎檢測）
	[[nodiscard]] bool IsPlayerInValidPosition() const; // 檢測玩家是否在合法位置
	[[nodiscard]] bool IsPlayerInCorridor(float epsilon = 16.0f) const; // 檢測玩家是否在通道上

	// 房間連接管理（委託給 RoomConnectionManager）
	void SetNeighborRoom(Direction dir, const std::weak_ptr<DungeonRoom> &neighbor, bool hasConnection = false);
	std::weak_ptr<DungeonRoom> GetNeighborRoom(Direction dir) const;
	bool HasConnectionToDirection(Direction dir) const;
	std::vector<std::weak_ptr<DungeonRoom>> GetConnectedNeighbors() const;
	std::vector<Direction> GetConnectedDirections() const;

	// 網格系統訪問（只讀）
	const std::vector<std::vector<int>> &GetGridData() const;
	bool IsGridPositionBlocked(int row, int col) const;

	// 地形生成
	void CreateCorridorInDirection(Direction dir);
	void CreateWallInDirection(Direction dir);

	// 房間設置完成後的最終處理
	void FinalizeRoomSetup();

	// 碰撞優化
	void OptimizeWallCollisions();

protected:
	// 碰撞優化輔助方法
	void RemoveWallCollisionComponents();
	std::vector<std::shared_ptr<nGameObject>> CreateOptimizedColliders(const std::vector<CollisionRect> &regions);

	// 調試功能
	void DebugDungeonRoom();

	// 初始化方法
	void InitializeGrid();

	// 地形創建輔助方法
	void CreateWall(int row, int col);
	void CreateFloor(int row, int col);
	void CreateDoor(int row, int col);

	// 成員變量
	RoomState m_State = RoomState::UNEXPLORED;
	RoomType m_RoomType;
	glm::vec2 m_MapGridPos = glm::vec2(0, 0);

	// 門對象容器
	std::vector<std::shared_ptr<nGameObject>> m_Doors;

	// 組合的管理器
	std::unique_ptr<GridSystem> m_GridSystem;
	std::unique_ptr<RoomConnectionManager> m_ConnectionManager;
	std::unique_ptr<TerrainGenerator> m_TerrainGenerator;

private:
	// 輔助方法
	[[nodiscard]] bool IsPlayerInCorridorDirection(const glm::vec2 &playerPos, const glm::vec2 &roomCenter,
												   const glm::vec2 &roomSize, const glm::vec2 &tileSize, Direction dir,
												   float epsilon) const;

	// 房間狀態變更處理
};

#endif // DUNGEONROOM_HPP
