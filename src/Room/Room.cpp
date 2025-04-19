//
// Created by QuzzS on 2025/3/7.
//

#include "Room/Room.hpp"

#include "Scene/SceneManager.hpp"
#include "Room/RoomInteractionManager.hpp"

Room::~Room() {
    // 析构函数 - 确保正确清理资源
    m_Characters.clear();
    m_RoomObjects.clear();
}

void Room::Start(const std::shared_ptr<Character>& player) {
	//更新缓存数据
	m_Player = player;
	UpdateCachedReferences();

	AddManager(ManagerTypes::ROOMCOLLISION,m_CollisionManager);
	AddManager(ManagerTypes::ROOMINTERACTIONMANAGER, m_InteractionManager);

	m_InteractionManager->SetPlayer(player);

    // 加载房间数据 TODO:要改 可能是读取head.json然后选择房间
    LoadFromJSON();
}

void Room::Update() {
    // 更新所有房间物体
    for (auto& obj : m_RoomObjects) {
        if (obj) obj->Update();
    }

    // 管理员动态逻辑
    m_CollisionManager->Update(); //效能暴鯉龍


	m_InteractionManager->Update();
	if (Util::Input::IsKeyDown(Util::Keycode::F)) m_InteractionManager->TryInteractWithClosest();

    // 注意:不在这里更新角色，因为角色更新应该由Scene负责
}

void Room::CharacterEnter(const std::shared_ptr<Character>& character) {
    if (character && !HasCharacter(character)) {
    	//加入生物组
        m_Characters.push_back(character);
    	//注册渲染器、相机、管理员
    	RegisterObjectToSceneAndManager(character);
		//触发进入反应 TODO:进入事件应该是Dungeon里面的房间大小
        OnCharacterEnter(character);
    }
}

void Room::CharacterExit(std::shared_ptr<Character> character) {
	if (!character) return;
	if (const auto it = std::find(m_Characters.begin(), m_Characters.end(), character);
		it != m_Characters.end()) {
        m_Characters.erase(it);
		UnRegisterObjectToSceneAndManager(character);
        OnCharacterExit(character);

    }
}

bool Room::HasCharacter(const std::shared_ptr<Character>& character) const {
    return std::find(m_Characters.begin(), m_Characters.end(), character) != m_Characters.end();
}

void Room::AddRoomObject(const std::shared_ptr<nGameObject>& object) {
    if (object) {
        m_RoomObjects.push_back(object);
    	RegisterObjectToSceneAndManager(object); //一键加入渲染器、相机和管理员
    }
}

void Room::RemoveRoomObject(const std::shared_ptr<nGameObject>& object) {
    if (object) {
    	UnRegisterObjectToSceneAndManager(object); //一键从渲染器、相机和管理员移除

        // 从列表移除
    	m_RoomObjects.erase(
			std::remove(m_RoomObjects.begin(), m_RoomObjects.end(), object),
			m_RoomObjects.end()
		);
    }
}

nlohmann::ordered_json Room::ReadJsonFile(const std::string &filePath) const
{
	std::ifstream file(filePath);
	if (!file.is_open()) {
		LOG_DEBUG("Error: Unable to open file: {}",filePath);
		return nlohmann::json();
	}

	nlohmann::json jsonData;
	file >> jsonData;
	return jsonData;
}

void Room::InitializeRoomObjects(const nlohmann::json &jsonData)
{
	m_RoomSpaceInfo.m_TileSize = glm::vec2(jsonData.at("tile_width").get<float>(), jsonData.at("tile_height").get<float>());
	m_RoomSpaceInfo.m_RoomRegion = glm::vec2(jsonData.at("room_region_x").get<float>(), jsonData.at("room_region_y").get<float>());
	m_RoomSpaceInfo.m_RoomSize = glm::vec2(jsonData.at("room_size_x").get<float>(), jsonData.at("room_size_y").get<float>());

	for (const auto &elem: jsonData["roomObject"]) {
		auto roomObject = m_Factory.lock()->createRoomObject(elem.at("ID").get<std::string>(), elem.at("Class").get<std::string>());
		if (roomObject) {
			const auto x = elem.at("Position")[0].get<float>();
			const auto y = elem.at("Position")[1].get<float>();
			const auto position = m_RoomSpaceInfo.m_WorldCoord + glm::vec2(x, y);
			roomObject->SetWorldCoord(position);
			AddRoomObject(roomObject);
		}
	}
}

void Room::UpdateCachedReferences()
{
	if (const std::shared_ptr<Scene> currentScene = SceneManager::GetInstance().GetCurrentScene().lock())
	{
		m_CachedCurrentScene = currentScene;
		m_CachedCamera = currentScene->GetCamera();
		m_CachedRenderer = currentScene->GetRoot();
	}
}

void Room::RegisterObjectToSceneAndManager(const std::shared_ptr<nGameObject> &object) const
{
	const auto renderer = m_CachedRenderer.lock();
	const auto camera = m_CachedCamera.lock();
	if (renderer && object->GetDrawable()) renderer->AddChild(object);
	if (camera) camera->AddChild(object);

	RegisterCollisionManger(object);
	RegisterInteractionManager(object);
}

void Room::RegisterCollisionManger(const std::shared_ptr<nGameObject>& object) const
{
	const auto renderer = m_CachedRenderer.lock();
	const auto camera = m_CachedCamera.lock();
	if (const auto collComp = object->GetComponent<CollisionComponent>(ComponentType::COLLISION)) {
		m_CollisionManager->RegisterNGameObject(object);
		if (const std::shared_ptr<nGameObject>& colliderVisible = collComp->GetVisibleBox())
		{
			if (renderer && colliderVisible->GetDrawable()) renderer->AddChild(colliderVisible);
			if (camera) camera->AddChild(colliderVisible);
		}
	}
}

void Room::RegisterInteractionManager(const std::shared_ptr<nGameObject> &object) const
{
	const auto renderer = m_CachedRenderer.lock();
	const auto camera = m_CachedCamera.lock();
	if (const auto interactComp = object->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
	{
		m_InteractionManager->RegisterInteractable(object);
		if (const std::shared_ptr<nGameObject>& promptObj = interactComp->GetPromptObject())
		{
			if (renderer) renderer->AddChild(promptObj);
			if (camera) camera->AddChild(promptObj);
		}
	}
}

void Room::UnRegisterObjectToSceneAndManager(const std::shared_ptr<nGameObject>& object) const
{
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	const auto renderer = scene->GetRoot().lock();
	const auto camera = scene->GetCamera().lock();
	if (scene) {
		if (renderer) renderer->RemoveChild(object);
		if (camera) camera->RemoveChild(object);
	}

	if (const auto collComp = object->GetComponent<CollisionComponent>(ComponentType::COLLISION)) {
		m_CollisionManager->UnregisterNGameObject(object);
		if (const std::shared_ptr<nGameObject>& colliderVisible = collComp->GetVisibleBox())
		{
			if (scene)
			{
				if (renderer) renderer->RemoveChild(colliderVisible);
				if (camera) camera->RemoveChild(colliderVisible);
			}
		}
	}

	if (const auto interactComp = object->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
	{
		m_InteractionManager->UnregisterInteractable(object);
		if (const std::shared_ptr<nGameObject>& promptObj = interactComp->GetPromptObject())
		{
			if (scene) {
				if (renderer) renderer->RemoveChild(promptObj);
				if (camera) camera->RemoveChild(promptObj);
			}
		}
	}
}

// void Room::RegisterTrackingManager(const std::shared_ptr<nGameObject> &object) const
// {
//
// }

