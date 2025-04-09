//
// Created by QuzzS on 2025/4/9.
//

#ifndef DUNGEONROOM_HPP
#define DUNGEONROOM_HPP

#include "Room.hpp"

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
	DungeonRoom();
	~DungeonRoom() override = default;

	// 重写基类方法
	void Start(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Character>& player) override;
	void Update() override;

	// 房间状态管理
	virtual void SetState(RoomState state);
	[[nodiscard]] RoomState GetState() const { return m_State; }

	void AddWallCollider(const std::shared_ptr<nGameObject>& collider);

protected:
	// 实现抽象方法
	void SetupWallColliders();

	// 重写状态变化处理方法 TODO:給子類別
	// void OnStateChanged(RoomState oldState, RoomState newState) override;

	// 重写角色进入/离开处理
	void OnCharacterEnter(const std::shared_ptr<Character>& character) override;
	void OnCharacterExit(const std::shared_ptr<Character>& character) override;

	virtual void OnStateChanged(RoomState oldState, RoomState newState) {}  // 房间状态变化时调用

	// 房间状态
	RoomState m_State = RoomState::INACTIVE;

private:
	bool m_PortalActive = false;  // 传送门是否激活
	// DungeonRoom類型 - 戰鬥房間（小怪，BOSS)， 特殊房間， 寶箱， 起始， 終點
	// Room狀態 -

};

#endif //DUNGEONROOM_HPP
