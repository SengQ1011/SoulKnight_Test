//
// Created by QuzzS on 2025/4/5.
//

#include "Room/LobbyRoom.hpp"

#include "Creature/Character.hpp"
#include "Components/CollisionComponent.hpp"
#include "Loader.hpp"
#include "Util/Logger.hpp"

void LobbyRoom::Start(const std::shared_ptr<Character>& player) {
    LOG_DEBUG("Initial LobbyRoom start");

    // 调用基类的Start方法
    Room::Start(player);

	// 設置大廳特有的墻壁碰撞體
	SetupWallColliders();
}

void LobbyRoom::Update() {
    // 调用基类Update
    Room::Update();

	// 更新所有墙壁碰撞体
	// for (auto& wall : m_WallColliders) {
	// 	if (wall) wall->Update();
	// }
}

void LobbyRoom::OnCharacterEnter(const std::shared_ptr<Character>& character) {
    LOG_DEBUG("Character  {} entered the lobby", character->GetName());

    // 处理角色进入大厅的特殊逻辑
    if (character->GetType() == CharacterType::PLAYER) {
        // 玩家进入大厅的特殊处理
        // 例如：播放背景音乐、触发NPC对话等
    }
}

void LobbyRoom::OnCharacterExit(const std::shared_ptr<Character>& character) {
    LOG_DEBUG("Character {} left the lobby", character->GetName());

    // 处理角色离开大厅的特殊逻辑
    if (character->GetType() == CharacterType::PLAYER) {
        // 玩家离开大厅的特殊处理
    }
}

void LobbyRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadLobbyObjectPosition();
	InitializeRoomObjects(jsonData);
}


void LobbyRoom::SetupWallColliders() {
	LOG_DEBUG("Set Lobby wall collider");

	// 根据预定义的偏移和尺寸创建墙壁碰撞体
	for (size_t i = 0; i < m_WallColliderOffsets.size(); i++) {
		auto wallCollider = std::make_shared<nGameObject>("LobbyWall_" + std::to_string(i));
		auto collisionComponent = wallCollider->AddComponent<CollisionComponent>(ComponentType::COLLISION);

		collisionComponent->SetOffset(m_WallColliderOffsets[i]);
		collisionComponent->SetSize(m_WallColliderSizes[i]);
		collisionComponent->SetCollisionLayer(CollisionLayers_Terrain);

		AddWallCollider(wallCollider);
	}
}

void LobbyRoom::AddWallCollider(const std::shared_ptr<nGameObject>& collider) {
	if (collider) {
		m_WallColliders.emplace_back(collider);

		RegisterCollisionManger(collider);
		RegisterTrackingManager(collider);
	}
}
