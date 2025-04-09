//
// Created by QuzzS on 2025/4/8.
//

#ifndef ROOMINTERACTIONMANAGER_HPP
#define ROOMINTERACTIONMANAGER_HPP

#include "Creature/Character.hpp"
#include "Override/nGameObject.hpp"
#include "Components/InteractableComponent.hpp"
class RoomInteractionManager {
public:
	RoomInteractionManager() = default;
	~RoomInteractionManager() = default;

	// 註冊/移除可互動物件
	void RegisterInteractable(const std::shared_ptr<nGameObject>& interactable);
	void UnregisterInteractable(const std::shared_ptr<nGameObject>& interactable);

	void Update(); // 更新互動提示（通常由Room::Update調用）
	[[nodiscard]] bool TryInteractWithClosest(float maxRadius = FLT_MAX) const; // Notify
	void SetPlayer(const std::shared_ptr<Character>& player) {m_Player = player;} // 引用玩家角色

protected:
	std::vector<std::weak_ptr<nGameObject>> m_InteractableObjects;
	std::weak_ptr<Character> m_Player;
	bool m_IsActive = true;

private:
	// 獲取最近的可互動物件
	[[nodiscard]] std::shared_ptr<nGameObject> GetClosestInteractable(float maxRadius) const;

	static void UpdateInteractable(const std::weak_ptr<nGameObject>& interactable, const std::shared_ptr<Character> &player);
};

#endif //ROOMINTERACTIONMANAGER_HPP
