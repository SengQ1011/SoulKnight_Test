//
// Created by QuzzS on 2025/3/16.
//

//RoomCollsionManager.cpp

#include "Util/Input.hpp"
#include "Room/RoomCollisionManager.hpp"
#include <execution> //并行計算
#include "Weapon/Bullet.hpp"
#include "Components/CollisionComponent.hpp"
#include "Util/Input.hpp"

void RoomCollisionManager::RegisterNGameObject(const std::shared_ptr<nGameObject>& nGameObject)
{
	LOG_DEBUG("RegisterNGameObject");
	if (auto collisionComp = nGameObject->GetComponent<CollisionComponent>(ComponentType::COLLISION);
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

void RoomCollisionManager::Update()
{
	if (!m_IsActive) return;
	std::vector<std::pair<std::shared_ptr<nGameObject>, std::shared_ptr<nGameObject>>> collisionPairs;

	// 调试：显示碰撞盒
	if (Util::Input::IsKeyUp(Util::Keycode::O)) {
		ShowColliderBox();
	}

	for (size_t i = 0; i < m_NGameObjects.size(); ++i) // ++i效率更好，都是從0開始，i++會建一個臨時變數
	{
		auto objectA = m_NGameObjects[i].lock();
		if (!objectA || !objectA->IsActive()) continue;

		auto colliderA = objectA->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		if (!colliderA) continue;

		for (size_t j = i + 1; j < m_NGameObjects.size(); ++j)
		{
			auto objectB = m_NGameObjects[j].lock();
			if (!objectB || !objectB->IsActive()) continue;

			auto colliderB = objectB->GetComponent<CollisionComponent>(ComponentType::COLLISION);
			if (!colliderB) continue;

			if (!colliderA->CanCollideWith(colliderB)) continue;
			Rect boundA = colliderA->GetBounds();
			Rect boundB = colliderB->GetBounds();

			if (boundA.Intersects(boundB)) collisionPairs.emplace_back(objectA, objectB);
		}
	}

	for (const auto& pair: collisionPairs) // 處理碰撞對
	{
		auto objectA = pair.first;
		auto objectB = pair.second;

		CollisionInfo info(objectA, objectB); // 創建碰撞信息

		CalculateCollisionDetails(objectA, objectB, info); // 計算碰撞詳情(穿透深度&碰撞法綫)

		DispatchCollision(objectA, objectB, info); // 分發碰撞處理
	}
}

void RoomCollisionManager::ShowColliderBox() // 房間内碰撞箱可視化
{
	m_IsVisible = m_IsVisible ^ true; //XOR bool 實現開關
	std::for_each(std::execution::par_unseq,m_NGameObjects.begin(), m_NGameObjects.end(),
		[&](const std::weak_ptr<nGameObject>& object)
		{
			if (const auto sharedPtr = object.lock())
				sharedPtr->GetComponent<CollisionComponent>(ComponentType::COLLISION)
				->GetVisibleBox()->SetVisible(m_IsVisible);
		});
}


void RoomCollisionManager::CalculateCollisionDetails(const std::shared_ptr<nGameObject>& objectA,
													 const std::shared_ptr<nGameObject>& objectB, CollisionInfo &info)
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
											 const std::shared_ptr<nGameObject> &objectB, CollisionInfo &info)
{
	const auto colliderA = objectA->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	const auto colliderB = objectB->GetComponent<CollisionComponent>(ComponentType::COLLISION);

	if ((colliderB->GetCollisionLayer() & colliderA->GetCollisionMask()) != 0) // !=運算符 優先於 &
	{
		objectA->onCollision(objectB,info);
	}
	if ((colliderA->GetCollisionLayer() & colliderB->GetCollisionMask()) != 0)
	{
		const glm::vec2 reversedNormal = -info.GetCollisionNormal();
		CollisionInfo reversedInfo(objectB, objectA);
		reversedInfo.penetration = info.penetration;
		reversedInfo.SetCollisionNormal(reversedNormal);

		objectB->onCollision(objectA, reversedInfo);
	}
}