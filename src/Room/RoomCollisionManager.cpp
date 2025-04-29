//
// Created by QuzzS on 2025/3/16.
//

//RoomCollsionManager.cpp

#include "Room/RoomCollisionManager.hpp"

#include <Tracy.hpp>
#include <execution>
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
		nGameObject && collisionComp) {
		m_NGameObjects.push_back(nGameObject);
	}else {
		LOG_ERROR("GameObject is null");
	}
}

void RoomCollisionManager::UnregisterNGameObject(const std::shared_ptr<nGameObject>& nGameObject)
{
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
}

// TODO： 沒有平行處理和空間分割
// void RoomCollisionManager::Update()
// {
// 	if (Util::Input::IsKeyUp(Util::Keycode::O)) {
// 		ShowColliderBox();
// 	}
//
// 	UniformGrid m_SpatialGrid;
// 	if (!m_IsActive) return;
//
// 	std::vector<std::pair<std::shared_ptr<nGameObject>, std::shared_ptr<nGameObject>>> collisionPairs;
//
// 	// 調試顯示碰撞盒
// 	if (Util::Input::IsKeyUp(Util::Keycode::O)) {
// 		ShowColliderBox();
// 	}
//
// 	// 檢查碰撞（暴力版本）
// 	for (size_t i = 0; i < m_NGameObjects.size(); ++i) {
// 		auto objectA = m_NGameObjects[i].lock();
// 		if (!objectA || !objectA->IsActive()) continue;
//
// 		auto colliderA = objectA->GetComponent<CollisionComponent>(ComponentType::COLLISION);
// 		if (!colliderA) continue;
//
// 		for (size_t j = i + 1; j < m_NGameObjects.size(); ++j) {
// 			auto objectB = m_NGameObjects[j].lock();
// 			if (!objectB || !objectB->IsActive()) continue;
//
// 			auto colliderB = objectB->GetComponent<CollisionComponent>(ComponentType::COLLISION);
// 			if (!colliderB) continue;
//
// 			if (!colliderA->CanCollideWith(colliderB)) continue;
//
// 			Rect boundA = colliderA->GetBounds();
// 			Rect boundB = colliderB->GetBounds();
//
// 			if (boundA.Intersects(boundB)) {
// 				collisionPairs.emplace_back(objectA, objectB);
// 			}
// 		}
// 	}
//
// 	// 處理碰撞
// 	for (const auto& pair : collisionPairs) {
// 		auto objectA = pair.first;
// 		auto objectB = pair.second;
//
// 		CollisionInfo info(objectA, objectB);
// 		CalculateCollisionDetails(objectA, objectB, info);
// 		DispatchCollision(objectA, objectB, info);
// 	}
// }

// void RoomCollisionManager::Update()
// {
//     ZoneScopedN("RoomManager::UpdateStart");
//
//     if (!m_IsActive) return;
//
//     std::vector<std::pair<std::shared_ptr<nGameObject>, std::shared_ptr<nGameObject>>> collisionPairs;
//     std::mutex mutex;
//
//     if (Util::Input::IsKeyUp(Util::Keycode::O)) {
//         ShowColliderBox();
//     }
//
//     {
//         ZoneScopedN("ClearSpatialGrid");
//         m_SpatialGrid.Clear();
//     }
//
//     {
//         ZoneScopedN("InsertToGrid");
//         for (const auto& weakObj : m_NGameObjects) {
//             auto obj = weakObj.lock();
//             if (!obj || !obj->IsActive())
//                 continue;
//
//             const std::shared_ptr<CollisionComponent> collider = obj->GetComponent<CollisionComponent>(ComponentType::COLLISION);
//             if (!collider) continue;
//
//             m_SpatialGrid.Insert(obj, collider->GetBounds());
//         }
//     }
//
//     {
//         ZoneScopedN("BroadPhaseCollision");
//         std::for_each(std::execution::par, m_NGameObjects.begin(), m_NGameObjects.end(), [&](const auto& weakObj) {
//             ZoneScopedN("NarrowPhaseAndRecord");
//
//             const std::shared_ptr<nGameObject> objectA = weakObj.lock();
//             if (!objectA || !objectA->IsActive())
//                 return;
//
//             const std::shared_ptr<CollisionComponent> colliderA = objectA->GetComponent<CollisionComponent>(ComponentType::COLLISION);
//             if (!colliderA || !colliderA->IsActive())
//                 return;
//
//             const Rect boundA = colliderA->GetBounds();
//             auto nearbyObjects = m_SpatialGrid.QueryNearby(boundA);
//
//             for (const auto& objectB : nearbyObjects) {
//                 if (objectA == objectB || !objectB->IsActive()) continue;
//                 if (objectA >= objectB) continue; // 去重複
//
//                 const std::shared_ptr<CollisionComponent> colliderB = objectB->GetComponent<CollisionComponent>(ComponentType::COLLISION);
//                 if (!colliderB || !colliderB->IsActive() || !colliderA->CanCollideWith(colliderB)) continue;
//
//                 if (colliderA->GetBounds().Intersects(colliderB->GetBounds())) {
//                     std::scoped_lock lock(mutex);
//                     collisionPairs.emplace_back(objectA, objectB);
//                 }
//             }
//         });
//     }
//
//     {
//         ZoneScopedN("HandleCollisions");
//         for (const auto& pair : collisionPairs) {
//             auto objectA = pair.first;
//             auto objectB = pair.second;
//
//             CollisionInfo info(objectA, objectB);
//             CalculateCollisionDetails(objectA, objectB, info);
//             DispatchCollision(objectA, objectB, info);
//         }
//     }
// }

void RoomCollisionManager::Update()
{
    ZoneScopedN("RoomManager::UpdateStart");

    if (!m_IsActive) return;

    std::vector<std::pair<std::shared_ptr<nGameObject>, std::shared_ptr<nGameObject>>> collisionPairs;
    std::mutex mutex;

    if (Util::Input::IsKeyUp(Util::Keycode::O)) {
        ShowColliderBox();
    }

    {
        ZoneScopedN("ClearSpatialGrid");
        m_SpatialGrid.Clear();
    }

    {
        ZoneScopedN("InsertToGrid");
        for (const auto& weakObj : m_NGameObjects) {
            auto obj = weakObj.lock();
            if (!obj || !obj->IsActive())
                continue;

            const std::shared_ptr<CollisionComponent> collider = obj->GetComponent<CollisionComponent>(ComponentType::COLLISION);
            if (!collider) continue;

            m_SpatialGrid.Insert(obj, collider->GetBounds());
        }
    }

    {
        ZoneScopedN("BroadPhaseCollision");
        std::for_each(std::execution::par, m_NGameObjects.begin(), m_NGameObjects.end(), [&](const auto& weakObj) {
            ZoneScopedN("NarrowPhaseAndRecord");

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
                if (!colliderB || !colliderB->IsActive() || !colliderA->CanCollideWith(colliderB)) continue;

                if (colliderA->GetBounds().Intersects(colliderB->GetBounds())) {
                    std::scoped_lock lock(mutex);
                    collisionPairs.emplace_back(objectA, objectB);
                }
            }
        });
    }

    {
        ZoneScopedN("HandleCollisions");
        for (const auto& pair : collisionPairs) {
            auto objectA = pair.first;
            auto objectB = pair.second;

        	CollisionEventInfo info(objectA, objectB);
            CalculateCollisionDetails(objectA, objectB, info);
            DispatchCollision(objectA, objectB, info);
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
			->GetVisibleBox()->SetVisible(m_IsVisible);
	}
}


// void RoomCollisionManager::CalculateCollisionDetails(const std::shared_ptr<nGameObject>& objectA,
// 													 const std::shared_ptr<nGameObject>& objectB, CollisionInfo &info)
// {
// 	auto colliderA = objectA->GetComponent<CollisionComponent>(ComponentType::COLLISION);
// 	auto colliderB = objectB->GetComponent<CollisionComponent>(ComponentType::COLLISION);
//
// 	if (!colliderA || !colliderB) return;
//
// 	Rect boundA = colliderA->GetBounds();
// 	Rect boundB = colliderB->GetBounds();
//
// 	// 計算四個方向的重叠
// 	float overlapLeft = boundB.right() - boundA.left();
// 	float overlapRight = boundA.right() - boundB.left();
// 	float overlapTop = boundA.top() - boundB.bottom();
// 	float overlapBottom = boundB.top() - boundA.bottom();
//
// 	// 找出最小重叠方向
// 	float minOverlapX = std::min(overlapLeft, overlapRight);
// 	float minOverlapY = std::min(overlapTop, overlapBottom);
//
// 	glm::vec2 normal(0.0f);
//
// 	// 決定碰撞法綫的方向
// 	if (minOverlapX < minOverlapY) {
// 		info.penetration = minOverlapX;
// 		normal.x = (overlapLeft < overlapRight) ? 1.0f : -1.0f;
// 	} else {
// 		info.penetration = minOverlapY;
// 		normal.y = (overlapTop < overlapBottom) ? -1.0f : 1.0f;
// 	}
//
// 	// 設置標準化的碰撞法綫
// 	info.SetCollisionNormal(normal);
// }

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

	if ((colliderB->GetCollisionLayer() & colliderA->GetCollisionMask()) != 0) // !=運算符 優先於 &
	{
		objectA->OnEvent(info); // TODO: debug
	}
	if ((colliderA->GetCollisionLayer() & colliderB->GetCollisionMask()) != 0)
	{
		const glm::vec2 reversedNormal = -info.GetCollisionNormal();
		CollisionEventInfo reversedInfo(objectB, objectA);
		reversedInfo.penetration = info.penetration;
		reversedInfo.SetCollisionNormal(reversedNormal);

		objectB->OnEvent(reversedInfo);
	}
}