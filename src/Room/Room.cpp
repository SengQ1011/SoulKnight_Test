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

void Room::Start(const std::shared_ptr<Character>& player) {
    LOG_DEBUG("Initial Room start");
	//更新緩存數據
	m_Player = player;
	UpdateCachedReferences();

	m_InteractionManager->SetPlayer(player);

    // 加载房间数据 TODO:要改 可能是讀取head.json然後選擇房間
    LoadFromJSON();
}

void Room::Update() {
    // 更新所有房间物体
    for (auto& obj : m_RoomObjects) {
        if (obj) obj->Update();
    }

    // 管理員動態邏輯
    if (m_CollisionManager) m_CollisionManager->Update();

	if (m_InteractionManager)
	{
		m_InteractionManager->Update();
		if (Util::Input::IsKeyDown(Util::Keycode::F)) m_InteractionManager->TryInteractWithClosest(FLT_MAX);
	}

    // 注意:不在这里更新角色，因为角色更新应该由Scene负责
}

//這是走進了房間區域RoomRegion觸發的
void Room::CharacterEnter(const std::shared_ptr<Character>& character) {
    if (character && !HasCharacter(character)) {
    	//加入生物組
        m_Characters.push_back(character);
    	//注冊渲染器、相機、管理員
    	RegisterObjectToSceneAndManager(character);
		//觸發進入反應 TODO:進入事件應該是Dungeon裏面的房間大小
        OnCharacterEnter(character);
    }
}

//這是走出了房間區域RoomRegion觸發的
void Room::CharacterExit(std::shared_ptr<Character> character) {
	if (!character) return;
	if (const auto it = std::find(m_Characters.begin(), m_Characters.end(), character);
		it != m_Characters.end()) {
        m_Characters.erase(it);
		UnRegisterObjectToSceneAndManager(character);
        OnCharacterExit(character);

    }
}

//判斷生物是否已經在房間記錄的容器裏面
bool Room::HasCharacter(const std::shared_ptr<Character>& character) const {
    return std::find(m_Characters.begin(), m_Characters.end(), character) != m_Characters.end();
}

void Room::AddRoomObject(const std::shared_ptr<RoomObject>& object) {
    if (object) {
        m_RoomObjects.push_back(object);

    	RegisterObjectToSceneAndManager(object); //一鍵加入渲染器、相機和管理員
    }
}

void Room::RemoveRoomObject(const std::shared_ptr<RoomObject>& object) {
    if (object) {
    	UnRegisterObjectToSceneAndManager(object); //一鍵從渲染器、相機和管理員移除

        // 从列表移除
    	m_RoomObjects.erase(
			std::remove(m_RoomObjects.begin(), m_RoomObjects.end(), object),
			m_RoomObjects.end()
		);
    }
}

void Room::LoadFromJSON() { // 根據圖紙創建
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadLobbyObjectPosition();
	InitializeRoomObjects(jsonData);
}

bool Room::IsPlayerInside() const
{
	const auto player = m_Player.lock();
	if (!player) return false;
	return (player->m_WorldCoord.x < m_WorldCoord.x + m_RoomRegion.x/2.0f) &&
		   (player->m_WorldCoord.y < m_WorldCoord.y + m_RoomRegion.y/2.0f) &&
		   (player->m_WorldCoord.x > m_WorldCoord.x - m_RoomRegion.x/2.0f) &&
		   (player->m_WorldCoord.y > m_WorldCoord.y - m_RoomRegion.y/2.0f);
}

//------------------------------------------ Protected Methods ------------------------------------------//
nlohmann::ordered_json Room::ReadJsonFile(const std::string &filePath) const
{
	std::ifstream file(filePath);
	if (!file.is_open()) {
		LOG_DEBUG("Error: Unable to open file: {}",filePath);
		return nlohmann::json();  // 如果文件打開失敗，返回空的 JSON 物件
	}

	nlohmann::json jsonData;
	file >> jsonData;
	return jsonData;
}

void Room::InitializeRoomObjects(const nlohmann::json &jsonData)
{
	// 设置房间尺寸
	m_RoomRegion.x = jsonData.at("room_width").get<float>() * jsonData.at("tile_width").get<float>();
	m_RoomRegion.y = jsonData.at("room_height").get<float>() * jsonData.at("tile_height").get<float>();
	m_TileSize = glm::vec2(jsonData.at("tile_width").get<float>(), jsonData.at("tile_height").get<float>());

	// 从JSON创建房间对象
	for (const auto &elem: jsonData["roomObject"]) {
		auto roomObject = m_Factory.lock()->createRoomObject(elem.at("ID").get<std::string>(), elem.at("Class").get<std::string>());
		if (roomObject) {
			const auto x = elem.at("Position")[0].get<float>();
			const auto y = elem.at("Position")[1].get<float>();
			const auto position = glm::vec2(x, y);
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

void Room::RegisterObjectToSceneAndManager(const std::shared_ptr<nGameObject> &object) const //查詢類成員
{
	// 将对象添加到场景根节点和相机
	const auto renderer = m_CachedRenderer.lock();
	const auto camera = m_CachedCamera.lock();
	if (renderer && object->GetDrawable()) renderer->AddChild(object);
	if (camera) camera->AddChild(object);

	RegisterCollisionManger(object); // 嘗試注冊到碰撞管理器
	RegisterInteractionManager(object); // 嘗試注冊到互動管理器
}

void Room::RegisterCollisionManger(const std::shared_ptr<nGameObject>& object) const
{
	const auto renderer = m_CachedRenderer.lock();
	const auto camera = m_CachedCamera.lock();
	// 如果对象有碰撞组件，注册到碰撞管理器
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
	// 如果有互動組件，注冊到互動管理器
	if (const auto interactComp = object->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
	{
		m_InteractionManager->RegisterInteractable(object);
		// 確保互動提示被添加到場景
		if (const std::shared_ptr<nGameObject>& promptObj = interactComp->GetPromptObject())
		{
			if (renderer) renderer->AddChild(promptObj);
			if (camera) camera->AddChild(promptObj);
		}
	}
}

void Room::UnRegisterObjectToSceneAndManager(const std::shared_ptr<nGameObject>& object) const
{
	// 将对象從场景根节点和相机刪除
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	const auto renderer = scene->GetRoot().lock();
	const auto camera = scene->GetCamera().lock();
	if (scene) {
		if (renderer) renderer->RemoveChild(object);
		if (camera) camera->RemoveChild(object);
	}

	// 从碰撞管理器移除
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

	// 从互動管理器移除
	if (const auto interactComp = object->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
	{
		m_InteractionManager->UnregisterInteractable(object);
		// 確保互動提示被刪除
		if (const std::shared_ptr<nGameObject>& promptObj = interactComp->GetPromptObject())
		{
			if (scene) {
				if (renderer) renderer->RemoveChild(promptObj);
				if (camera) camera->RemoveChild(promptObj);
			}
		}
	}
}







