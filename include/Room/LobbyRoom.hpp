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
	explicit LobbyRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader>& loader, const std::shared_ptr<RoomObjectFactory>& room_object_factory)
	: Room(worldCoord, loader, room_object_factory) {}
	~LobbyRoom() override = default;

	// 重写基类方法
	void Start(const std::shared_ptr<Character>& player) override;
	void Update() override;

	void AddWallCollider(const std::shared_ptr<nGameObject>& collider);
	std::vector<std::shared_ptr<nGameObject>> GetWallColliders() {return m_WallColliders; };

protected:
	// 实现抽象方法
	void SetupWallColliders();

	// 重写角色进入/离开处理
	void OnCharacterEnter(const std::shared_ptr<Character>& character) override;
	void OnCharacterExit(const std::shared_ptr<Character>& character) override;

	//LobbyRoom有自己的讀取方式
	void LoadFromJSON() override;

private:
	// Lobby特有数据
	bool m_PortalActive = false;  // 传送门是否激活

	// Lobby特有墙壁碰撞体配置
	std::vector<std::shared_ptr<nGameObject>> m_WallColliders;    // 墙壁碰撞体

	const std::vector<glm::vec2> m_WallColliderOffsets = {
		glm::vec2(-296.0f,0.0f),
		glm::vec2(-160.0f,144.0f),
		glm::vec2(0.0f,152.0f),
		glm::vec2(160.0f,144.0f),
		glm::vec2(296.0f,0.0f),
		glm::vec2(0.0f,-200.0f)
	};

	const std::vector<glm::vec2> m_WallColliderSizes = {
		glm::vec2(16.0f, 384.0f),
		glm::vec2(256.0f, 96.0f),
		glm::vec2(64.0f, 80.0f),
		glm::vec2(256.0f, 96.0f),
		glm::vec2(16.0f, 384.0f),
		glm::vec2(608.0f, 16.0f)
	};

};

#endif //LOBBYROOM_HPP