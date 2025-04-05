//
// Created by QuzzS on 2025/3/7.
//
//
// Created by QuzzS on 2025/3/7.
//

#include "Room/Room.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Logger.hpp"

Room::Room() {
    // 默认构造函数
}

Room::~Room() {
    // 析构函数 - 确保正确清理资源
    m_Characters.clear();
    m_RoomObjects.clear();
}

void Room::Start(std::shared_ptr<Camera> camera) {
    LOG_DEBUG("初始化房间");
    m_Camera = camera;

    // 加载房间数据
    LoadFromJSON(JSON_DIR"/LobbyObjectPosition.json");

    // 注册碰撞
    RegisterCollisions();

    // 设置初始状态
    SetState(RoomState::ACTIVE);
}

void Room::Update() {
    // 更新房间状态
    UpdateRoomState();

    // 更新所有房间物体
    for (auto& obj : m_RoomObjects) {
        if (obj) obj->Update();
    }

    // 更新碰撞检测
    if (m_CollisionManager) {
        m_CollisionManager->UpdateCollision();
    }

    // 注意：不在这里更新角色，因为角色更新应该由Scene负责
}

void Room::SetState(RoomState state) {
    if (m_State != state) {
        RoomState oldState = m_State;
        m_State = state;
        OnStateChanged(oldState, state);
    }
}

void Room::UpdateRoomState() {
    // 基类中的默认实现 - 子类可以重写
    // 例如检查房间内是否还有敌人，以决定是否从COMBAT切换到CLEARED
}

void Room::CharacterEnter(std::shared_ptr<Character> character) {
    if (character && !HasCharacter(character)) {
        m_Characters.push_back(character);
        OnCharacterEnter(character);

        // 如果是玩家，激活房间
        if (character->GetType() == CharacterType::PLAYER) {
            if (m_State == RoomState::INACTIVE) {
                SetState(RoomState::ACTIVE);
            }
        }
    }
}

void Room::CharacterExit(std::shared_ptr<Character> character) {
    auto it = std::find(m_Characters.begin(), m_Characters.end(), character);
    if (it != m_Characters.end()) {
        m_Characters.erase(it);
        OnCharacterExit(character);

        // 如果没有玩家，将房间设置为非活动状态
        bool hasPlayer = std::any_of(m_Characters.begin(), m_Characters.end(),
            [](const std::shared_ptr<Character>& c) { return c->GetType() == CharacterType::PLAYER; });

        if (!hasPlayer && m_State == RoomState::ACTIVE) {
            SetState(RoomState::INACTIVE);
        }
    }
}

bool Room::HasCharacter(const std::shared_ptr<Character>& character) const {
    return std::find(m_Characters.begin(), m_Characters.end(), character) != m_Characters.end();
}

void Room::AddRoomObject(const std::shared_ptr<RoomObject>& object) {
    if (object) {
        m_RoomObjects.push_back(object);

        // 将对象添加到场景根节点和相机
        auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
        if (scene) {
            scene->GetRoot().lock()->AddChild(object);

            if (auto camera = m_Camera.lock()) {
                camera->AddRelativePivotChild(object);
            }
        }

        // 如果对象有碰撞组件，注册到碰撞管理器
        if (auto collComp = object->GetComponent<CollisionComponent>(ComponentType::COLLISION)) {
            m_CollisionManager->RegisterNGameObject(object);
        }
    }
}

void Room::RemoveRoomObject(const std::shared_ptr<RoomObject>& object) {
    if (object) {
        // 从碰撞管理器移除
        if (object->GetComponent<CollisionComponent>(ComponentType::COLLISION)) {
            m_CollisionManager->UnregisterNGameObject(object);
        }

        // 从场景和相机移除
        auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
        if (scene) {
            scene->GetRoot().lock()->RemoveChild(object);

            if (auto camera = m_Camera.lock()) {
                camera->RemoveRelativePivotChild(object);
            }
        }

        // 从列表移除
        auto it = std::find(m_RoomObjects.begin(), m_RoomObjects.end(), object);
        if (it != m_RoomObjects.end()) {
            m_RoomObjects.erase(it);
        }
    }
}

void Room::LoadFromJSON(const std::string& jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        LOG_DEBUG("错误: 无法打开文件: {}", jsonFilePath);
        return;
    }

    nlohmann::json jsonData;
    file >> jsonData;

    // 设置房间尺寸
    m_RoomHeight = jsonData.at("room_height").get<float>() * jsonData.at("tile_height").get<float>();
    m_Size.x = jsonData.at("room_width").get<float>() * jsonData.at("tile_width").get<float>();
    m_Size.y = m_RoomHeight;
    m_TileSize = glm::vec2(jsonData.at("tile_width").get<float>(), jsonData.at("tile_height").get<float>());

    // 设置相机地图大小
    if (auto camera = m_Camera.lock()) {
        camera->SetMapSize(m_RoomHeight);
    }

    // 从JSON创建房间对象
    for (const auto &elem: jsonData["roomObject"]) {
        auto roomObject = m_Factory->createRoomObject(elem.at("ID").get<std::string>(), elem.at("Class").get<std::string>());
        if (roomObject) {
            const auto x = elem.at("Position")[0].get<float>();
            const auto y = elem.at("Position")[1].get<float>();
            const auto position = glm::vec2(x, y);
            roomObject->SetWorldCoord(position);
            AddRoomObject(roomObject);
            LOG_DEBUG("创建房间对象，位置: {}", position);
        }
    }
}

void Room::RegisterCollisions() {
	// 注册有碰撞组件的房间对象
	for (const auto& obj : m_RoomObjects) {
		if (!obj) continue;

		auto collComp = obj->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		if (!collComp) continue;

		m_CollisionManager->RegisterNGameObject(obj);

		auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
		if (!scene) continue;

		scene->GetRoot().lock()->AddChild(collComp->GetBlackBox());

		auto camera = m_Camera.lock();
		if (!camera) continue;

		camera->AddRelativePivotChild(collComp->GetBlackBox());
	}
}