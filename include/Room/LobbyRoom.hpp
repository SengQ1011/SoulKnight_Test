//
// Created by QuzzS on 2025/4/5.
//

#ifndef LOBBYROOM_HPP
#define LOBBYROOM_HPP

#include "Room/Room.hpp"
#include "Components/MovementComponent.hpp"
#include "Creature/Character.hpp"

class LobbyRoom : public Room {
public:
	LobbyRoom();
	~LobbyRoom() override = default;

	// 重写基类方法
	void Start(std::shared_ptr<Camera> camera) override;
	void Update() override;

	// Lobby特有功能
	void SetupNPCs();
	void ActivatePortal(bool active);
	bool IsPortalActive() const { return m_PortalActive; }

	void AddWallCollider(const std::shared_ptr<nGameObject>& collider);

protected:
	// 实现抽象方法
	void SetupWallColliders();

	// 重写状态变化处理方法
	void OnStateChanged(RoomState oldState, RoomState newState) override;

	// 重写角色进入/离开处理
	void OnCharacterEnter(const std::shared_ptr<Character>& character) override;
	void OnCharacterExit(const std::shared_ptr<Character>& character) override;

private:
	// Lobby特有数据
	bool m_PortalActive = false;  // 传送门是否激活
	std::shared_ptr<RoomObject> m_Portal = nullptr;  // 传送门对象


	// Lobby特有墙壁碰撞体配置
	std::vector<std::shared_ptr<nGameObject>> m_WallColliders;    // 墙壁碰撞体

	const std::vector<glm::vec2> m_WallColliderOffsets = {
		glm::vec2(-304.0f, -192.0f),
		glm::vec2(-288.0f, 96.0f),
		glm::vec2(-32.0f, 112.0f),
		glm::vec2(32.0f, 96.0f),
		glm::vec2(288.0f, -192.0f),
		glm::vec2(-304.0f, -208.0f)
	};

	const std::vector<glm::vec2> m_WallColliderSizes = {
		glm::vec2(16.0f, 384.0f),
		glm::vec2(256.0f, 96.0f),
		glm::vec2(64.0f, 80.0f),
		glm::vec2(256.0f, 96.0f),
		glm::vec2(16.0f, 384.0f),
		glm::vec2(608.0f, 16.0f)
	};

	// Lobby特有方法
	void SetupInteractiveObjects();
	void HandleInteractions();
};

#endif //LOBBYROOM_HPP