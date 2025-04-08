//
// Created by QuzzS on 2025/4/8.
//

#ifndef ROOMINTERACTIONMANAGER_HPP
#define ROOMINTERACTIONMANAGER_HPP

#include "Creature/Character.hpp"
#include "Override/nGameObject.hpp"
class RoomInteractionManager {
public:
	RoomInteractionManager() = default;
	~RoomInteractionManager() = default;

	// 註冊/移除可互動物件
	void RegisterInteractable(const std::shared_ptr<nGameObject>& interactable);
	void UnregisterInteractable(const std::shared_ptr<nGameObject>& interactable);

	// 獲取最近的可互動物件
	[[nodiscard]] std::shared_ptr<nGameObject> GetClosestInteractable(
		const glm::vec2& position, float maxRadius) const;

	// 顯示/隱藏所有互動提示
	void ShowInteractionHints(bool show);

	// 更新互動提示（通常由Room::Update調用）
	void UpdateInteractionHints(const glm::vec2& playerPosition, float interactionRadius);

	// 清空所有註冊的互動物件（通常在房間被卸載時調用）
	void Clear();

	// 調試用：顯示互動範圍
	void ToggleDebugVisibility();


protected:
	std::vector<std::weak_ptr<nGameObject>> m_InteractableObjects;
	std::weak_ptr<Character> m_Player;
	bool isVisible = true;

};

#endif //ROOMINTERACTIONMANAGER_HPP
