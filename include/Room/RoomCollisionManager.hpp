//
// Created by QuzzS on 2025/3/16.
//

//RoomCollisionManager.hpp

#ifndef ROOMCOLLISIONMANAGER_HPP
#define ROOMCOLLISIONMANAGER_HPP

#include "ObserveManager/IManager.hpp"
#include "Room/UniformGrid.hpp"


struct CollisionEventInfo;
class nGameObject;
struct CollisionInfo;

class RoomCollisionManager : public IManager {
public:
	RoomCollisionManager() {m_SpatialGrid.Initialize(560, 560, 32); };
	~RoomCollisionManager() override = default;

	// 注冊監聽成員
	void RegisterNGameObject(const std::shared_ptr<nGameObject>& nGameObject);
	void UnregisterNGameObject(const std::shared_ptr<nGameObject>& nGameObject);

	void Update() override; // 更新碰撞情況

	//是否啓動管理員
	void SetIsActive(const bool isActive) {m_IsActive = isActive;}
	[[nodiscard]] bool IsActive() const { return m_IsActive; }

protected:
	UniformGrid m_SpatialGrid;
	std::vector<std::weak_ptr<nGameObject>> m_NGameObjects;
	std::vector<std::weak_ptr<nGameObject>> m_TriggerObjects; // 扳機子集局部更新
	bool m_IsVisible = true; // 記錄碰撞箱顯示
	bool m_IsActive = true;

private:

	static void CalculateCollisionDetails(const std::shared_ptr<nGameObject> &objectA,
										  const std::shared_ptr<nGameObject> &objectB,
										  CollisionEventInfo &info);

	static void DispatchCollision(const std::shared_ptr<nGameObject>& objectA,
								  const std::shared_ptr<nGameObject>& objectB,
								  CollisionEventInfo & info);
};

#endif //ROOMCOLLISIONMANAGER_HPP
