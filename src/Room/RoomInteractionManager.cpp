//
// Created by QuzzS on 2025/4/8.
//

#include "Room/RoomInteractionManager.hpp"
#include "Components/InteractableComponent.hpp"
#include "Override/nGameObject.hpp"
#include "Creature/Character.hpp"
#include "Util/Logger.hpp"

void RoomInteractionManager::RegisterInteractable(const std::shared_ptr<nGameObject> &interactable)
{
	if (interactable && interactable->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE)) {
		m_InteractableObjects.push_back(interactable);
	}
}

void RoomInteractionManager::UnregisterInteractable(const std::shared_ptr<nGameObject> &interactable)
{
	m_InteractableObjects.erase(
		std::remove_if(m_InteractableObjects.begin(), m_InteractableObjects.end(),
			[&interactable](const auto& weakPtr)
			{
				if (weakPtr.expired()) return true; //檢查weak_ptr是否過期
				const auto sharedPtr = weakPtr.lock(); //轉換成shared_ptr來比較
				return sharedPtr.get() == interactable.get(); //比較指針地址
			}),
		m_InteractableObjects.end()
	);
}

std::shared_ptr<nGameObject> RoomInteractionManager::GetClosestInteractable(float maxRadius) const
{
	auto player = m_Player.lock();
	if (!player) return nullptr;
	// 記錄最靠近的object和距離
	std::shared_ptr<nGameObject> closestInteractable = nullptr;
	float closestDistance = maxRadius;

	for (const auto& weakPtr : m_InteractableObjects) // 不能并行因爲會改變外面值
	{
		LOG_DEBUG("RoomInteractionManager::GetClosestInteractable");
		if (weakPtr.expired()) continue;
		const auto obj = weakPtr.lock();
		if (!obj) continue;

		const auto component = obj->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE);
		if (!component) continue;

		if (const float distance = glm::length(obj->GetWorldCoord() - player->GetWorldCoord()); //取得之間距離
			distance < closestDistance && distance <= component->GetInteractionRadius()) //小於最靠近距離 在互動範圍裏
		{
			closestInteractable = obj;
			closestDistance = distance;
		}
	}
	return closestInteractable;
}

void RoomInteractionManager::UpdateAutoInteractions() {
	const auto player = m_Player.lock();
	if (!player) return;

	for (const auto& weakInteractable : m_InteractableObjects) {
		auto interactable = weakInteractable.lock();
		if (!interactable || !interactable->IsActive()) continue;

		const auto comp = interactable->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE);
		if (comp && comp->IsAutoInteract() && comp->IsInRange(player)) {
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

void RoomInteractionManager::Update() // 玩家位置的更新 來判斷是否顯示互動提示
{
	if (!m_IsActive) return;
	auto player = m_Player.lock();
	if (!player) return;

	// 對每個interactable做更新
	for (const auto& interactable : m_InteractableObjects) UpdateInteractable(interactable,player);

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




