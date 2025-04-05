//
// Created by QuzzS on 2025/4/5.
//

#include "Room/LobbyRoom.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Factory/CharacterFactory.hpp"

LobbyRoom::LobbyRoom() : Room() {
    // LobbyRoom特有的初始化
}

void LobbyRoom::Start(std::shared_ptr<Camera> camera) {
    LOG_DEBUG("Initial LobbyRoom start");

    // 调用基类的Start方法
    Room::Start(camera);

	// 設置大廳特有的墻壁碰撞體
	SetupWallColliders();

    // 设置大厅特有的互动对象
    SetupInteractiveObjects();

    // 设置NPC
    SetupNPCs();

    // 将房间设置为ACTIVE状态
    SetState(RoomState::ACTIVE);
}

void LobbyRoom::Update() {
    // 调用基类Update
    Room::Update();

    // 处理大厅特有的交互
    HandleInteractions();

    // 调试：显示碰撞盒
    if (Util::Input::IsKeyUp(Util::Keycode::O)) {
        m_CollisionManager->ShowColliderBox();
    }

	// 更新所有墙壁碰撞体
	for (auto& wall : m_WallColliders) {
		if (wall) wall->Update();
	}
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

void LobbyRoom::SetupInteractiveObjects() {
    // 设置大厅中的互动对象，例如传送门、工作台等
    // 这些对象可能有特殊的交互功能

    // 创建传送门（示例）
    auto portalFactory = m_Factory->createRoomObject("object_dungeonDoor", "RoomObject");
    if (portalFactory) {
        m_Portal = portalFactory;
        m_Portal->SetWorldCoord(glm::vec2(0.0f, 128.0f));

        // 添加传送门的交互组件（这里只是示例，实际需要实现交互组件）
        // m_Portal->AddComponent<InteractionComponent>(ComponentType::INTERACTION);

        AddRoomObject(m_Portal);
    }

    // 设置为默认未激活状态
    ActivatePortal(false);
}

void LobbyRoom::SetupNPCs() {
    // 在大厅中设置NPC
    // 例如：商人、任务NPC等

    // 示例：创建一个商人NPC
    // auto merchant = CharacterFactory::GetInstance().createNPC("Merchant");
    // merchant->SetWorldCoord(glm::vec2(120.0f, 50.0f));
    // CharacterEnter(merchant);
}

void LobbyRoom::HandleInteractions() {
    // 处理大厅特有的交互逻辑
    // 例如检测玩家是否接近互动对象，并提示交互选项

    // 检查玩家是否接近传送门
    bool playerNearPortal = false;

    for (const auto& character : m_Characters) {
        if (character->GetType() == CharacterType::PLAYER && m_Portal) {
            // 简化的距离检查
            float distance = glm::length(character->GetWorldCoord() - m_Portal->GetWorldCoord());
            if (distance < 50.0f) {  // 交互距离阈值
                playerNearPortal = true;
                break;
            }
        }
    }

    // 处理传送门交互
    if (playerNearPortal && Util::Input::IsKeyUp(Util::Keycode::E)) {
        // 这里可以触发进入地牢的逻辑
        LOG_DEBUG("Player-Portal Interaction");
        // 例如，可以通知SceneManager切换到地牢场景
    }
}

void LobbyRoom::ActivatePortal(bool active) {
    m_PortalActive = active;

    // 更新传送门的视觉效果和交互状态
    if (m_Portal) {
        // 示例：更改传送门的外观
        // 实际实现中，可能需要切换动画或更改纹理
        // m_Portal->SetAnimation(active ? "active" : "inactive");

        LOG_DEBUG("Portal state changed to: {}", active ? "Activated" : "Deactivated");
    }
}

void LobbyRoom::OnStateChanged(RoomState oldState, RoomState newState) {
    LOG_DEBUG("Lobby room state changed from {} to {}", static_cast<int>(oldState), static_cast<int>(newState));

    // 处理状态变化的特殊逻辑
    switch (newState) {
        case RoomState::ACTIVE:
            // 大厅被激活时的处理
            break;
        case RoomState::INACTIVE:
            // 大厅变为非活动状态时的处理
            break;
        default:
            break;
    }
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

void LobbyRoom::AddWallCollider(const std::shared_ptr<nGameObject>& collider) {
	if (collider) {
		m_WallColliders.emplace_back(collider);

		// 注册到碰撞管理器
		m_CollisionManager->RegisterNGameObject(collider);

		// 获取碰撞组件并将其黑盒添加到场景和相机
		if (auto collComp = collider->GetComponent<CollisionComponent>(ComponentType::COLLISION)) {
			auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
			if (scene) {
				scene->GetRoot().lock()->AddChild(collComp->GetBlackBox());

				if (auto camera = m_Camera.lock()) {
					camera->AddRelativePivotChild(collComp->GetBlackBox());
				}
			}
		}
	}
}
