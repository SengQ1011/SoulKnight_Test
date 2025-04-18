//
// Created by QuzzS on 2025/3/16.
//

//RoomCollisionManager.hpp

#ifndef ROOMCOLLISIONMANAGER_HPP
#define ROOMCOLLISIONMANAGER_HPP

#include "pch.hpp"
#include "Override/nGameObject.hpp"

class RoomCollisionManager {
public:
	RoomCollisionManager() = default;
	~RoomCollisionManager() = default;

	// 注冊監聽成員
	void RegisterNGameObject(const std::shared_ptr<nGameObject>& nGameObject);
	void UnregisterNGameObject(const std::shared_ptr<nGameObject>& nGameObject);

	void Update(); // 更新碰撞情況
	void ShowColliderBox(); // 一鍵顯示當前房間碰撞箱

	//是否啓動管理員
	void SetIsActive(const bool isActive) {m_IsActive = isActive;}
	[[nodiscard]] bool IsActive() const {return m_IsActive;}

protected:
	std::vector<std::weak_ptr<nGameObject>> m_NGameObjects;
	bool m_IsVisible = true; // 記錄碰撞箱顯示
	bool m_IsActive = true;

private:
	// 計算碰撞詳情
	static void CalculateCollisionDetails(const std::shared_ptr<nGameObject>& objectA,
								  const std::shared_ptr<nGameObject>& objectB,
								  CollisionInfo& info);

	// 分發碰撞處理
	static void DispatchCollision(const std::shared_ptr<nGameObject>& objectA,
						  const std::shared_ptr<nGameObject>& objectB,
						  CollisionInfo& info);
};

#endif //ROOMCOLLISIONMANAGER_HPP
