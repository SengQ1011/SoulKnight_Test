//
// Created by QuzzS on 2025/4/9.
//

#ifndef DUNGEONROOM_HPP
#define DUNGEONROOM_HPP

#include "Factory/RoomFactory.hpp"
#include "Room.hpp"

struct Rect;

// 房间状态枚举
enum class RoomState {
	INACTIVE,   // 未激活（玩家不在该房间）
	ACTIVE,     // 已激活（玩家在该房间内）
	COMBAT,     // 战斗中
	CLEARED,    // 已清理（战斗结束或无战斗房间）
	SPECIAL     // 特殊状态（宝箱房、商店等）
};

class DungeonRoom : public Room {
public:
	explicit DungeonRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader>& loader, const std::shared_ptr<RoomObjectFactory>& room_object_factory)
	: Room(worldCoord, loader, room_object_factory) {};
	~DungeonRoom() override = default;

	// 重写基类方法
	void Start(const std::shared_ptr<Character>& player) override;
	void Update() override;

	// 房间状态管理
	void SetState(RoomState state) {m_State = state;};
	RoomState GetState() const { return m_State; }

	std::string GetRoomType() const { return m_RoomType; }
	[[nodiscard]] bool IsPlayerInside() const; //場景使用的 確認當前玩家所在的房間

	void LoadFromJSON() override;

	float IntersectionArea(const Rect& a, const Rect& b);
	void CreateGridAndVisibleGrid();
	void CreateGridAndVisibleGridPar();



protected:

	// 重写状态变化处理方法 TODO:給子類別
	// void OnStateChanged(RoomState oldState, RoomState newState) override;

	// 重写角色进入/离开处理
	// void OnCharacterEnter(const std::shared_ptr<Character>& character) override;
	// void OnCharacterExit(const std::shared_ptr<Character>& character) override;

	// 更新房间状态的辅助方法
	// virtual void UpdateRoomState();

	virtual void OnStateChanged(RoomState oldState, RoomState newState)
	{

	}  // 房间状态变化时调用

	// 房间状态
	RoomState m_State = RoomState::INACTIVE;
	std::shared_ptr<nGameObject> m_Bound3535;

private:
	std::string m_RoomType = "START";
	//網格處理
	std::vector<std::vector<std::shared_ptr<nGameObject>>> m_Grid = std::vector<std::vector<std::shared_ptr<nGameObject>>>(
		35,std::vector<std::shared_ptr<nGameObject>>(35)); //用來可視化debug的
	std::vector<std::vector<int>> m_Mark = std::vector<std::vector<int>>(35,std::vector<int>(35,0)); //用來記錄處理的資料

	// DungeonRoom類型 - 戰鬥房間（小怪，BOSS)， 特殊房間， 寶箱， 起始， 終點
	// Room狀態 -

};

#endif //DUNGEONROOM_HPP
