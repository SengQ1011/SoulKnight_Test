//
// Created by QuzzS on 2025/3/16.
//

//RoomCollsionManager.cpp

#include "Room/RoomCollisionManager.hpp"
#include <iostream>
#include "Components/CollisionComponent.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"

#include "Room/UniformGrid.hpp"

void RoomCollisionManager::RegisterNGameObject(const std::shared_ptr<nGameObject>& nGameObject)
{
	// LOG_DEBUG("RegisterNGameObject");
	if (!nGameObject) {
		LOG_ERROR("GameObject is null");
		return;
	}
	if (const auto collisionComp = nGameObject->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		collisionComp) {
		m_NGameObjects.push_back(nGameObject);
		if (collisionComp->IsTrigger()) m_TriggerObjects.push_back(nGameObject);
	}else {
		LOG_ERROR("GameObject is null");
	}
}

void RoomCollisionManager::UnregisterNGameObject(const std::shared_ptr<nGameObject>& nGameObject)
{
	// 移除一般 GameObject
	m_NGameObjects.erase(
		std::remove_if(m_NGameObjects.begin(), m_NGameObjects.end(),
			[&nGameObject](const auto& weakPtr)
			{
				if (weakPtr.expired()) return false; //檢查weak_ptr是否過期
				const auto sharedPtr = weakPtr.lock(); //轉換成shared_ptr來比較
				return sharedPtr.get() == nGameObject.get(); //比較指針地址
			}),
		m_NGameObjects.end()
	);

	// 移除 Trigger GameObject（子集）
	m_TriggerObjects.erase(
		std::remove_if(m_TriggerObjects.begin(), m_TriggerObjects.end(),
			[&nGameObject](const auto& weakPtr)
			{
				if (weakPtr.expired()) return false;
				const auto sharedPtr = weakPtr.lock();
				return sharedPtr.get() == nGameObject.get();
			}),
		m_TriggerObjects.end()
	);
}

void RoomCollisionManager::Update()
{
    if (!m_IsActive) return;

    std::vector<std::pair<std::shared_ptr<nGameObject>, std::shared_ptr<nGameObject>>> collisionPairs;
    std::mutex mutex;

    if (Util::Input::IsKeyUp(Util::Keycode::O)) {
        ShowColliderBox();
    }

    m_SpatialGrid.Clear();

    for (const auto& weakObj : m_NGameObjects) {
        auto obj = weakObj.lock();
        if (!obj || !obj->IsActive())
            continue;

        const std::shared_ptr<CollisionComponent> collider = obj->GetComponent<CollisionComponent>(ComponentType::COLLISION);
        if (!collider) continue;

        m_SpatialGrid.Insert(obj, collider->GetBounds());
    }


    std::for_each(m_NGameObjects.begin(), m_NGameObjects.end(), [&](const auto& weakObj) {

        const std::shared_ptr<nGameObject> objectA = weakObj.lock();
        if (!objectA || !objectA->IsActive())
            return;

        const std::shared_ptr<CollisionComponent> colliderA = objectA->GetComponent<CollisionComponent>(ComponentType::COLLISION);
        if (!colliderA || !colliderA->IsActive())
            return;

        const Rect boundA = colliderA->GetBounds();
        auto nearbyObjects = m_SpatialGrid.QueryNearby(boundA);

        for (const auto& objectB : nearbyObjects) {
            if (objectA == objectB || !objectB->IsActive()) continue;
            if (objectA >= objectB) continue; // 去重複

            const std::shared_ptr<CollisionComponent> colliderB = objectB->GetComponent<CollisionComponent>(ComponentType::COLLISION);

            // 判斷是否能發生碰撞，不能跳過
            if (!colliderB || !colliderB->IsActive()) continue;
			const bool aCanCollideWithB = colliderA->CanCollideWith(colliderB);
            const bool bCanCollideWithA = colliderB->CanCollideWith(colliderA);
            if (!(aCanCollideWithB || bCanCollideWithA)) continue;

            if (colliderA->GetBounds().Intersects(colliderB->GetBounds())) {
                std::scoped_lock lock(mutex);
                collisionPairs.emplace_back(objectA, objectB);
            }
        }
    });

    for (const auto& pair : collisionPairs) {
        auto objectA = pair.first;
        auto objectB = pair.second;
        CollisionEventInfo info(objectA, objectB);
        CalculateCollisionDetails(objectA, objectB, info);
        DispatchCollision(objectA, objectB, info);
    }

    for (const auto& weakObj : m_TriggerObjects)
    {
    	if (const auto obj = weakObj.lock())
    	{
			if (const auto collider = obj->GetComponent<CollisionComponent>(ComponentType::COLLISION);
				collider && collider->IsActive())
    		{
    			collider->FinishTriggerFrame(obj);
    		}
		}
    }

}

void RoomCollisionManager::ShowColliderBox() // 房間内碰撞箱可視化
{
	m_IsVisible = m_IsVisible ^ true; //XOR bool 實現開關
	for (const std::weak_ptr<nGameObject>& object: m_NGameObjects)
	{
		if (const auto sharedPtr = object.lock())
			sharedPtr->GetComponent<CollisionComponent>(ComponentType::COLLISION)
			->GetVisibleBox()->SetControlVisible(m_IsVisible);
	}
}

void RoomCollisionManager::CalculateCollisionDetails(const std::shared_ptr<nGameObject>& objectA,
													 const std::shared_ptr<nGameObject>& objectB, CollisionEventInfo &info)
{
	auto colliderA = objectA->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	auto colliderB = objectB->GetComponent<CollisionComponent>(ComponentType::COLLISION);

	if (!colliderA || !colliderB) return;

	Rect boundA = colliderA->GetBounds();
	Rect boundB = colliderB->GetBounds();

	// 計算四個方向的重叠
	float overlapLeft = boundB.right() - boundA.left();
	float overlapRight = boundA.right() - boundB.left();
	float overlapTop = boundA.top() - boundB.bottom();
	float overlapBottom = boundB.top() - boundA.bottom();

	// 找出最小重叠方向
	float minOverlapX = std::min(overlapLeft, overlapRight);
	float minOverlapY = std::min(overlapTop, overlapBottom);

	glm::vec2 normal(0.0f);

	// 決定碰撞法綫的方向
	if (minOverlapX < minOverlapY) {
		info.penetration = minOverlapX;
		normal.x = (overlapLeft < overlapRight) ? 1.0f : -1.0f;
	} else {
		info.penetration = minOverlapY;
		normal.y = (overlapTop < overlapBottom) ? -1.0f : 1.0f;
	}

	// 設置標準化的碰撞法綫
	info.SetCollisionNormal(normal);
}

void RoomCollisionManager::DispatchCollision(const std::shared_ptr<nGameObject> &objectA,
											 const std::shared_ptr<nGameObject> &objectB,
											 CollisionEventInfo &info)
{
	const auto colliderA = objectA->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	const auto colliderB = objectB->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	const bool canAHitB  = colliderA->CanCollideWith(colliderB);
	const bool canBHitA  = colliderB->CanCollideWith(colliderA);
	const bool aIsTrigger = colliderA->IsTrigger();
	const bool bIsTrigger = colliderB->IsTrigger();
	const bool aIsCollider = colliderA->IsCollider();
	const bool bIsCollider = colliderB->IsCollider();

	// 扳機碰撞觸發
	if (aIsTrigger || bIsTrigger)
	{
		if (aIsTrigger && canAHitB) colliderA->TryTrigger(objectA, objectB);
		if (bIsTrigger && canBHitA) colliderB->TryTrigger(objectB, objectA);
	}
	if (aIsCollider && bIsCollider)
	{
		if (canAHitB) objectA->OnEvent(info);
		if (canBHitA) {
			const glm::vec2 reversedNormal = -info.GetCollisionNormal();
			CollisionEventInfo reversedInfo(objectB, objectA);
			reversedInfo.penetration = info.penetration;
			reversedInfo.SetCollisionNormal(reversedNormal);
			objectB->OnEvent(reversedInfo);
		}
	}
}