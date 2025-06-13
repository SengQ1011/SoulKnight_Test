//
// Created by QuzzS on 2025/4/8.
//

#include "Room/RoomInteractionManager.hpp"
#include "Components/InteractableComponent.hpp"
#include "Creature/Character.hpp"
#include "Override/nGameObject.hpp"
#include "Util/Logger.hpp"


void RoomInteractionManager::RegisterInteractable(const std::shared_ptr<nGameObject> &interactable)
{
	if (!interactable)
		return;
	const auto comp = interactable->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE);
	if (!comp)
		return;

	// 檢查是否已經註冊，避免重複註冊
	for (const auto &entry : m_InteractableObjects)
	{
		if (!entry.obj.expired())
		{
			auto obj = entry.obj.lock();
			if (obj && obj.get() == interactable.get())
			{
				LOG_DEBUG("Interactable already registered, skipping: {}", interactable->GetName());
				return;
			}
		}
	}

	// LOG_DEBUG("Successfully registered interactable: {}", interactable->GetName());
	InteractableEntry entry;
	entry.obj = interactable;
	entry.comp = comp; // shared_ptr → weak_ptr 自動轉換
	m_InteractableObjects.push_back(entry);
}


void RoomInteractionManager::UnregisterInteractable(const std::shared_ptr<nGameObject> &interactable)
{
	auto oldSize = m_InteractableObjects.size();
	// 有找到才刪除
	m_InteractableObjects.erase(std::remove_if(m_InteractableObjects.begin(), m_InteractableObjects.end(),
											   [&interactable](const InteractableEntry &entry)
											   {
												   auto obj = entry.obj.lock();
												   return !obj || obj.get() == interactable.get();
											   }),
								m_InteractableObjects.end());

	if (m_InteractableObjects.size() == oldSize)
	{
		LOG_WARN("RoomInteractionManager::UnregisterInteractable - target not found or already expired");
	}
}

std::shared_ptr<nGameObject> RoomInteractionManager::GetClosestInteractable(float maxRadius) const
{
	auto player = m_Player.lock();
	if (!player)
		return nullptr;

	// 記錄最靠近的object和距離
	std::shared_ptr<nGameObject> closestInteractable = nullptr;
	float closestDistance = maxRadius;

	for (const auto &entry : m_InteractableObjects)
	{
		// 先檢查 weak_ptr 是否已經失效
		if (entry.obj.expired() || entry.comp.expired())
			continue;

		auto obj = entry.obj.lock();
		auto comp = entry.comp.lock();
		if (!obj || !comp || !comp->IsComponentActive())
			continue;

		try
		{
			// 修正：兩個 nGameObject 之間的距離計算
			const float distance = glm::length(obj->GetWorldCoord() - player->GetWorldCoord());
			if (distance < closestDistance && distance <= comp->GetInteractionRadius())
			{
				closestInteractable = obj;
				closestDistance = distance;
			}
		}
		catch (...)
		{
			LOG_WARN("Exception occurred while calculating distance for object: {}", obj->GetName());
		}
	}
	return closestInteractable;
}

void RoomInteractionManager::UpdateAutoInteractions() const
{
	const auto player = m_Player.lock();
	if (!player)
		return;

	for (const auto &entry : m_InteractableObjects)
	{
		// 先檢查 weak_ptr 是否已經失效
		if (entry.obj.expired() || entry.comp.expired())
			continue;

		auto obj = entry.obj.lock();
		auto comp = entry.comp.lock();
		if (!obj || !comp || !obj->IsActive())
			continue;

		try
		{
			if (comp->IsAutoInteract() && comp->IsInRange(player))
			{
				comp->OnInteract(player);
			}
		}
		catch (...)
		{
			LOG_ERROR("Exception occurred in UpdateAutoInteractions for object: {}", obj->GetName());
		}
	}
}

bool RoomInteractionManager::TryInteractWithClosest(float maxRadius) const
{
	LOG_DEBUG("TryInteractWithClosest");
	const auto player = m_Player.lock();
	if (!player)
		return false;

	// 尋找最近的可互動物件
	const auto closestObj = GetClosestInteractable(maxRadius);
	if (!closestObj)
		return false;

	// 執行互動
	const auto component = closestObj->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE);
	if (component)
		return component->OnInteract(player);
	return false;
}

void RoomInteractionManager::SetPlayer(const std::shared_ptr<Character> &player) { m_Player = player; }

void RoomInteractionManager::Update()
{
	if (!m_IsActive)
		return;
	const auto player = m_Player.lock();
	if (!player)
		return;

	// 先收集需要處理的有效物件，避免在迭代過程中修改容器
	std::vector<std::pair<std::shared_ptr<nGameObject>, std::shared_ptr<InteractableComponent>>> validObjects;

	// 第一階段：收集有效物件並清理失效物件
	for (auto it = m_InteractableObjects.begin(); it != m_InteractableObjects.end();)
	{
		// 先檢查 weak_ptr 是否已經失效
		if (it->obj.expired() || it->comp.expired())
		{
			LOG_DEBUG("Removing expired interactable object");
			it = m_InteractableObjects.erase(it);
			continue;
		}

		auto obj = it->obj.lock();
		auto comp = it->comp.lock();

		// 雙重檢查：確保 lock() 成功
		if (!obj || !comp)
		{
			LOG_DEBUG("Removing invalid interactable object after lock");
			it = m_InteractableObjects.erase(it);
			continue;
		}

		// 收集有效物件
		validObjects.emplace_back(obj, comp);
		++it;
	}

	// 第二階段：使用現有方法找到最靠近的可互動物件
	std::shared_ptr<nGameObject> closestInteractable = GetClosestInteractable(1000.0f);

	// 第三階段：處理所有物件的顯示和自動互動
	for (const auto &[obj, comp] : validObjects)
	{
		try
		{
			// 再次檢查物件是否仍然有效
			if (!obj || !comp || !obj->IsActive() || !comp->IsComponentActive())
				continue;

			const bool inRange = comp->IsInRange(player);

			// 只有最靠近的物件才顯示提示
			const bool shouldShowPrompt = inRange && (obj == closestInteractable);
			comp->ShowPrompt(shouldShowPrompt);

			// 若是自動互動的，直接觸發
			if (inRange && comp->IsAutoInteract())
				comp->OnInteract(player);
		}
		catch (...)
		{
			LOG_ERROR("Exception occurred while processing interactable object: {}", obj->GetName());
			// 將有問題的物件加入待移除列表
			m_ToUnregister.push_back(obj);
		}
	}

	// 處理待移除的物件
	for (const auto &obj : m_ToUnregister)
	{
		UnregisterInteractable(obj);
	}
	m_ToUnregister.clear();
}

void RoomInteractionManager::UpdateInteractable(const std::weak_ptr<nGameObject> &interactable,
												const std::shared_ptr<Character> &player)
{
	if (!player)
		return;
	if (interactable.expired())
		return;
	const auto obj = interactable.lock();
	const auto component = obj->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE);
	if (!component)
		return;

	// 當玩家進入範圍
	const bool inRange = component->IsInRange(player);
	component->ShowPrompt(inRange);
	if (inRange && component->IsAutoInteract())
		component->OnInteract(player); // 自動觸發
}
