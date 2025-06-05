//
// Created by QuzzS on 2025/4/9.
//

#ifndef DUNGEONROOM_HPP
#define DUNGEONROOM_HPP

#include "Factory/RoomFactory.hpp"
#include "Room.hpp"

struct Rect;

// 前向聲明
class DungeonRoom;

// 房间状态枚举
enum class RoomState
{
	UNEXPLORED, // 未探索
	EXPLORED, // 已探索
	COMBAT, // 战斗中
	CLEANED, // 已清除
};

enum class Direction
{
	UP = 0,
	RIGHT = 1,
	DOWN = 2,
	LEFT = 3
};

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

// 房間連接結構，記錄四個方向的相鄰房間
struct RoomConnection
{
	std::array<std::weak_ptr<DungeonRoom>, 4> neighbors; // 四個方向的相鄰房間
	std::array<bool, 4> hasConnection = {false, false, false, false}; // 是否有通道連接

	// 輔助方法
	std::weak_ptr<DungeonRoom> GetNeighbor(Direction dir) const { return neighbors[static_cast<int>(dir)]; }

	void SetNeighbor(Direction dir, const std::weak_ptr<DungeonRoom> &room) { neighbors[static_cast<int>(dir)] = room; }

	bool HasConnection(Direction dir) const { return hasConnection[static_cast<int>(dir)]; }

	void SetConnection(Direction dir, bool connected) { hasConnection[static_cast<int>(dir)] = connected; }

	// 獲取所有有連接的相鄰房間
	std::vector<std::weak_ptr<DungeonRoom>> GetConnectedNeighbors() const
	{
		std::vector<std::weak_ptr<DungeonRoom>> connected;
		for (int i = 0; i < 4; ++i)
		{
			if (hasConnection[i] && !neighbors[i].expired())
			{
				connected.push_back(neighbors[i]);
			}
		}
		return connected;
	}

	// 獲取所有有連接的方向
	std::vector<Direction> GetConnectedDirections() const
	{
		std::vector<Direction> directions;
		for (int i = 0; i < 4; ++i)
		{
			if (hasConnection[i])
			{
				directions.push_back(static_cast<Direction>(i));
			}
		}
		return directions;
	}
};

class DungeonRoom : public Room
{
public:
	explicit DungeonRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader> &loader,
						 const std::shared_ptr<RoomObjectFactory> &room_object_factory,
						 const glm::vec2 &mapGridPos = glm::vec2(0, 0), const RoomType roomType = RoomType::DUNGEON) :
		Room(worldCoord, loader, room_object_factory), m_MapGridPos(mapGridPos), m_RoomType(roomType)
	{
	}
	~DungeonRoom() override = default;

	// 重写基类方法
	void Start(const std::shared_ptr<Character> &player) override;
	void Update() override;

	// 房间状态管理
	virtual void TryActivateByPlayer() { m_State = RoomState::EXPLORED; }
	virtual void OnStateChanged() {} // 房间状态变化时會發生的事情
	void SetState(RoomState newState)
	{
		if (m_State != newState)
		{
			m_State = newState;
			OnStateChanged();
		}
	}
	RoomState GetState() const { return m_State; }

	std::vector<std::vector<int>> GetMark() const { return m_Mark; }
	RoomType GetRoomType() const { return m_RoomType; }
	glm::vec2 GetMapGridPos() const { return m_MapGridPos; }
	[[nodiscard]] bool IsPlayerInsideRegion() const; // 場景使用的 確認當前玩家所在的房間
	[[nodiscard]] bool IsPlayerInsideRoom() const; // 房間使用的 處罰房間互動 關門放狗之類的

	// 房間連接管理
	const RoomConnection &GetRoomConnection() const { return m_RoomConnection; }
	void SetNeighborRoom(Direction dir, const std::weak_ptr<DungeonRoom> &neighbor, bool hasConnection = false);
	std::weak_ptr<DungeonRoom> GetNeighborRoom(Direction dir) const;
	bool HasConnectionToDirection(Direction dir) const;
	std::vector<std::weak_ptr<DungeonRoom>> GetConnectedNeighbors() const;
	std::vector<Direction> GetConnectedDirections() const;

	void LoadFromJSON() override;

	float IntersectionArea(const Rect &a, const Rect &b);
	void CreateGrid();
	void CreateCorridorInDirection(Direction dir);
	void CreateWallInDirection(Direction dir);

	// 重写角色进入/离开处理
	// void OnCharacterEnter(const std::shared_ptr<Character>& character) override;
	// void OnCharacterExit(const std::shared_ptr<Character>& character) override;

	// 更新房间状态的辅助方法
	// virtual void UpdateRoomState();

	void DebugDungeonRoom();

protected:
	void CreateWall(int row, int col);
	void CreateFloor(int row, int col);
	void CreateDoor(int row, int col);

	// 重写状态变化处理方法 TODO:給子類別
	// void OnStateChanged(RoomState oldState, RoomState newState) override;

	// 房间状态
	RoomState m_State = RoomState::UNEXPLORED;
	glm::vec2 m_MapGridPos = glm::vec2(0, 0); // 房間在地圖的哪個位置
	std::vector<std::shared_ptr<nGameObject>> m_Doors;

	// 網格處理
	std::vector<std::vector<int>> m_Mark =
		std::vector<std::vector<int>>(35, std::vector<int>(35, 0)); // 用來記錄處理的資料
private:
	RoomType m_RoomType;

	// DungeonRoom類型 - 戰鬥房間（小怪，BOSS)， 特殊房間， 寶箱， 起始， 終點
	// Room狀態 -
	RoomConnection m_RoomConnection;
};

#endif // DUNGEONROOM_HPP
