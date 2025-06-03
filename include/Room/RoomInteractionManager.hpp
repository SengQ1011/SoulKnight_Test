//
// Created by QuzzS on 2025/4/8.
//

#ifndef ROOMINTERACTIONMANAGER_HPP
#define ROOMINTERACTIONMANAGER_HPP

#include <memory>
#include <vector>

#include "ObserveManager/IManager.hpp"


class InteractableComponent;
class nGameObject;
class Character;

struct InteractableEntry {
	std::weak_ptr<nGameObject> obj;
	std::weak_ptr<InteractableComponent> comp;
};

class RoomInteractionManager : public IManager {
public:
	RoomInteractionManager() = default;
	~RoomInteractionManager() = default;

	// 註冊/移除可互動物件
	void RegisterInteractable(const std::shared_ptr<nGameObject>& interactable);
	void UnregisterInteractable(const std::shared_ptr<nGameObject>& interactable);
	void QueueUnregister(const std::shared_ptr<nGameObject>& obj) { m_ToUnregister.push_back(obj); }

	void Update() override; // 更新互動提示（通常由Room::Update調用）
	void UpdateAutoInteractions() const;
	bool TryInteractWithClosest(float maxRadius = 50.0f) const; // Notify
	void SetPlayer(const std::shared_ptr<Character>& player); // 引用玩家角色

protected:
	std::vector<InteractableEntry> m_InteractableObjects;
	std::vector<std::shared_ptr<nGameObject>> m_ToUnregister;

	std::weak_ptr<Character> m_Player;
	bool m_IsActive = true;

private:
	// 獲取最近的可互動物件
	[[nodiscard]] std::shared_ptr<nGameObject> GetClosestInteractable(float maxRadius) const;

	static void UpdateInteractable(const std::weak_ptr<nGameObject>& interactable, const std::shared_ptr<Character> &player);
};

#endif //ROOMINTERACTIONMANAGER_HPP
