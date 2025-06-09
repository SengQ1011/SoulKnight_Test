//
// Created by QuzzS on 2025/4/24.
//

#include "Components/DoorComponent.hpp"

#include <iostream>
#include "Override/nGameObject.hpp"
#include "Structs/EventInfo.hpp"
#include "Util/Image.hpp"

DoorComponent::DoorComponent()
{
	// 在建構函數中訂閱事件
	m_DoorOpenListenerID = EventManager::GetInstance().Subscribe<DoorOpenEvent>([this](const DoorOpenEvent &event)
																				{ this->DoorOpened(); });

	m_DoorCloseListenerID = EventManager::GetInstance().Subscribe<DoorCloseEvent>([this](const DoorCloseEvent &event)
																				  { this->DoorClosed(); });
}

DoorComponent::~DoorComponent()
{
	// 在解構函數中取消訂閱事件
	// 添加安全檢查，避免在程序結束時存取已銷毀的EventManager
	try
	{
		// 檢查 EventManager 是否仍然有效
		if (!EventManager::IsValid())
		{
// 使用 std::cout 進行調試輸出（相對安全）
#ifdef _DEBUG
			try
			{
				std::cout << "DoorComponent::~DoorComponent: EventManager is being destroyed, skipping unsubscribe"
						  << std::endl;
			}
			catch (...)
			{
				// 靜默處理輸出異常
			}
#endif
			return;
		}

		if (m_DoorOpenListenerID != 0)
		{
			EventManager::GetInstance().Unsubscribe<DoorOpenEvent>(m_DoorOpenListenerID);
			m_DoorOpenListenerID = 0;
		}
		if (m_DoorCloseListenerID != 0)
		{
			EventManager::GetInstance().Unsubscribe<DoorCloseEvent>(m_DoorCloseListenerID);
			m_DoorCloseListenerID = 0;
		}
	}
	catch (...)
	{
		// 不使用 LOG_WARN，因為 spdlog 可能已經被銷毀
		// 在析構函數中靜默處理異常
	}
}

void DoorComponent::Init()
{
	auto &imagePoolManger = ImagePoolManager::GetInstance();
	const auto drawable0 = imagePoolManger.GetImage(RESOURCE_DIR "/IcePlains/object_door_0.png");
	const auto drawable1 = imagePoolManger.GetImage(RESOURCE_DIR "/IcePlains/object_door_1.png");
	m_drawables.emplace_back(drawable0);
	m_drawables.emplace_back(drawable1);
	DoorOpened();
}

void DoorComponent::Update() {}

void DoorComponent::DoorOpened()
{
	const std::shared_ptr<nGameObject> owner = GetOwner<nGameObject>();
	if (!owner)
		return;
	const std::shared_ptr<CollisionComponent> collisionComp =
		owner->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (!collisionComp)
		return;
	owner->SetDrawable(m_drawables[0]);
	owner->SetZIndexType(OBJECTLOW);
	collisionComp->SetActive(false);
	m_currentState = State::OPENED;
}

void DoorComponent::DoorClosed()
{
	const std::shared_ptr<nGameObject> owner = GetOwner<nGameObject>();
	if (!owner)
		return;
	const std::shared_ptr<CollisionComponent> collisionComp =
		owner->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (!collisionComp)
		return;
	owner->SetDrawable(m_drawables[1]);
	owner->SetZIndexType(OBJECTHIGH);
	collisionComp->SetActive(true);
	m_currentState = State::CLOSED;
}
