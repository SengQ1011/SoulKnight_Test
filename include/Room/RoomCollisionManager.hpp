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

	// 注冊監聽成員
	void RegisterNGameObject(const std::shared_ptr<nGameObject>& nGameObject);
	void UnregisterNGameObject(const std::shared_ptr<nGameObject>& nGameObject);

	void UpdateCollision() const;

	void ShowColliderBox();

private:
	// 計算碰撞詳情
	static void CalculateCollisionDetails(const std::shared_ptr<nGameObject>& objectA,
								  const std::shared_ptr<nGameObject>& objectB,
								  CollisionInfo& info);

	// 分發碰撞處理
	static void DispatchCollision(const std::shared_ptr<nGameObject>& objectA,
						  const std::shared_ptr<nGameObject>& objectB,
						  CollisionInfo& info);

protected:
	std::vector<std::shared_ptr<nGameObject>> m_NGameObjects;
	bool isVisible = true;
};

#endif //ROOMCOLLISIONMANAGER_HPP
