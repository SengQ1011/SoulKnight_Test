//
// Created by QuzzS on 2025/4/5.
//

#include "Room/LobbyRoom.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Factory/CharacterFactory.hpp"

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

    // 调试：显示碰撞盒
    if (Util::Input::IsKeyUp(Util::Keycode::O)) {
        m_CollisionManager->ShowColliderBox();
    }

	// 更新所有墙壁碰撞体
	for (auto& wall : m_WallColliders) {
		if (wall) wall->Update();
	}
}

// TODO:給Dungeon子類別
// void LobbyRoom::OnStateChanged(RoomState oldState, RoomState newState) {
//     LOG_DEBUG("Lobby room state changed from {} to {}", static_cast<int>(oldState), static_cast<int>(newState));
//
//     // 处理状态变化的特殊逻辑
//     switch (newState) {
//         case RoomState::ACTIVE:
//             // 大厅被激活时的处理
//             break;
//         case RoomState::INACTIVE:
//             // 大厅变为非活动状态时的处理
//             break;
//         default:
//             break;
//     }
// }

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
		// // 注册到碰撞管理器
		// m_CollisionManager->RegisterNGameObject(collider);
		//
		// // 获取碰撞组件并将其黑盒添加到场景和相机
		// if (auto collComp = collider->GetComponent<CollisionComponent>(ComponentType::COLLISION)) {
		// 	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
		// 	if (scene) {
		// 		scene->GetRoot().lock()->AddChild(collComp->GetVisibleBox());
		//
		// 		if (auto camera = scene->GetCamera().lock()) {
		// 			camera->AddChild(collComp->GetVisibleBox());
		// 		}
		// 	}
		// }
	}
}
