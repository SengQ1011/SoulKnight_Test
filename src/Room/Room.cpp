//
// Created by QuzzS on 2025/3/7.
//
//
// Created by QuzzS on 2025/3/7.
//

#include "Room/Room.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Logger.hpp"


Room::~Room() {
    // 析构函数 - 确保正确清理资源
    m_Characters.clear();
    m_RoomObjects.clear();
}

void Room::Start(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Character>& player) {
    LOG_DEBUG("Initial Room start");
    m_Camera = camera;
	m_Player = player;

	m_InteractionManager->SetPlayer(player);

    // 加载房间数据 TODO:要改 可能是讀取head.json然後選擇房間
	LOG_DEBUG("Initial Room end{}",m_ThemePath);
    LoadFromJSON(m_ThemePath+"/ObjectPosition.json");
}

void Room::Update() {
    // 更新房间状态
    UpdateRoomState();

    // 更新所有房间物体
    for (auto& obj : m_RoomObjects) {
        if (obj) obj->Update();
    }

    // 更新碰撞检测
    if (m_CollisionManager) m_CollisionManager->UpdateCollision();

	if (m_InteractionManager)
	{
		m_InteractionManager->Update();
		if (Util::Input::IsKeyDown(Util::Keycode::F)) m_InteractionManager->TryInteractWithClosest(FLT_MAX);
	}

    // 注意：不在这里更新角色，因为角色更新应该由Scene负责
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
        if (character->GetType() != CharacterType::PLAYER) return;
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

    }
}

bool Room::HasCharacter(const std::shared_ptr<Character>& character) const {
    return std::find(m_Characters.begin(), m_Characters.end(), character) != m_Characters.end();
}

void Room::AddRoomObject(const std::shared_ptr<RoomObject>& object) {
    if (object) {
        m_RoomObjects.push_back(object);

        // 将对象添加到场景根节点和相机
		const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
    	const auto renderer = scene->GetRoot().lock();
    	const auto camera = scene->GetCamera().lock();
        if (scene) {
            if (renderer && object->GetDrawable()) renderer->AddChild(object);
        	if (camera) camera->AddChild(object);
        }

        // 如果对象有碰撞组件，注册到碰撞管理器
        if (auto collComp = object->GetComponent<CollisionComponent>(ComponentType::COLLISION)) {
            m_CollisionManager->RegisterNGameObject(object);
        	if (const std::shared_ptr<nGameObject>& colliderVisible = collComp->GetVisibleBox())
        	{
        		if (scene)
        		{
        			if (renderer && colliderVisible->GetDrawable()) renderer->AddChild(colliderVisible);
        			if (camera) camera->AddChild(colliderVisible);
        		}
        	}
        }

    	// 如果有互動組件，注冊到互動管理器
    	if (auto interactComp = object->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
    	{
    		m_InteractionManager->RegisterInteractable(object);
    		// 確保互動提示被添加到場景
    		if (const std::shared_ptr<nGameObject>& promptObj = interactComp->GetPromptObject())
    		{
    			if (scene) {
    				if (renderer) renderer->AddChild(promptObj);
    				if (camera) camera->AddChild(promptObj);
    			}
    		}
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
                camera->RemoveChild(object);
            }
        }

        // 从列表移除
        auto it = std::find(m_RoomObjects.begin(), m_RoomObjects.end(), object);
        if (it != m_RoomObjects.end()) {
            m_RoomObjects.erase(it);
        }
    }
}

void Room::LoadFromJSON(const std::string& jsonFilePath) { // 根據圖紙創建
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        LOG_DEBUG("Error: can't open in Room: {}", jsonFilePath);
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
        }
    }
}
