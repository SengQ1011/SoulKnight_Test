//
// Created by QuzzS on 2025/4/8.
//

#include "Room/RoomInteractionManager.hpp"
#include "Components/InteractableComponent.hpp"
#include "Override/nGameObject.hpp"
#include "Creature/Character.hpp"
#include "Util/Logger.hpp"

void RoomInteractionManager::RegisterInteractable(const std::shared_ptr<nGameObject>& interactable)
{
	if (!interactable) return;
	const auto comp = interactable->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE);
	if (!comp) return;
	LOG_DEBUG("Successfully registered interactable");
	InteractableEntry entry;
	entry.obj = interactable;
	entry.comp = comp; // shared_ptr → weak_ptr 自動轉換
	m_InteractableObjects.push_back(entry);
}


void RoomInteractionManager::UnregisterInteractable(const std::shared_ptr<nGameObject> &interactable)
{
	auto oldSize = m_InteractableObjects.size();
	// 有找到才刪除
	m_InteractableObjects.erase(
		std::remove_if(m_InteractableObjects.begin(), m_InteractableObjects.end(),
			[&interactable](const InteractableEntry& entry)
			{
				auto obj = entry.obj.lock();
				return !obj || obj.get() == interactable.get();
			}),
		m_InteractableObjects.end()
	);

	if (m_InteractableObjects.size() == oldSize)
	{
		LOG_WARN("RoomInteractionManager::UnregisterInteractable - target not found or already expired");
	}
}

std::shared_ptr<nGameObject> RoomInteractionManager::GetClosestInteractable(float maxRadius) const
{
	auto player = m_Player.lock();
	if (!player) return nullptr;
	// 記錄最靠近的object和距離
	std::shared_ptr<nGameObject> closestInteractable = nullptr;
	float closestDistance = maxRadius;

	for (const auto& entry : m_InteractableObjects) // 不能并行因爲會改變外面值
	{
		LOG_DEBUG("RoomInteractionManager::GetClosestInteractable");
		auto obj = entry.obj.lock();
		auto comp = entry.comp.lock();
		if (!obj || !comp) continue;

		if (const float distance = glm::length(obj->GetWorldCoord() - player->GetWorldCoord()); //取得之間距離
			distance < closestDistance && distance <= comp->GetInteractionRadius()) //小於最靠近距離 在互動範圍裏
		{
			closestInteractable = obj;
			closestDistance = distance;
		}
	}
	return closestInteractable;
}

void RoomInteractionManager::UpdateAutoInteractions() const
{
	const auto player = m_Player.lock();
	if (!player) return;

	for (const auto& entry : m_InteractableObjects) {
		auto obj = entry.obj.lock();
		auto comp = entry.comp.lock();
		if (!obj || !comp || !obj->IsActive()) continue;

		if (comp->IsAutoInteract() && comp->IsInRange(player)) {
			comp->OnInteract(player);
		}
	}
}

bool RoomInteractionManager::TryInteractWithClosest(float maxRadius) const
{
	LOG_DEBUG("TryInteractWithClosest");
	const auto player = m_Player.lock();
	if (!player) return false;

	// 尋找最近的可互動物件
	const auto closestObj = GetClosestInteractable(maxRadius);
	if (!closestObj) return false;

	// 執行互動
	const auto component = closestObj->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE);
	if (component) return component->OnInteract(player);
	return false;
}

void RoomInteractionManager::SetPlayer(const std::shared_ptr<Character> &player) { m_Player = player; }

void RoomInteractionManager::Update()
{
	if (!m_IsActive) return;
	const auto player = m_Player.lock();
	if (!player) return;

	for (auto& entry : m_InteractableObjects)
	{
		auto obj = entry.obj.lock();
		auto comp = entry.comp.lock();

		if (!obj || !comp) continue;

		// 範圍判斷與提示顯示
		const bool inRange = comp->IsInRange(player);
		comp->ShowPrompt(inRange);

		// 若是自動互動的，直接觸發
		if (inRange && comp->IsAutoInteract())
			comp->OnInteract(player);
	}
	for (const auto& obj : m_ToUnregister) {
		UnregisterInteractable(obj);
	}
	m_ToUnregister.clear();

}

void RoomInteractionManager::UpdateInteractable(const std::weak_ptr<nGameObject>& interactable,
												 const std::shared_ptr<Character> &player)
{
	if (!player) return;
	if (interactable.expired()) return;
	const auto obj = interactable.lock();
	const auto component = obj->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE);
	if (!component) return;

	// 當玩家進入範圍
	const bool inRange = component->IsInRange(player);
	component->ShowPrompt(inRange);
	if (inRange && component->IsAutoInteract()) component->OnInteract(player); //自動觸發
}