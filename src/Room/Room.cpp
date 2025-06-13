//
// Created by QuzzS on 2025/3/7.
//

#include "Room/Room.hpp"

#include <iostream>

#include "Components/ChestComponent.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/InteractableComponent.hpp"
#include "Creature/Character.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Factory/WeaponFactory.hpp"
#include "RandomUtil.hpp"
#include "Room/RoomCollisionManager.hpp"
#include "Room/RoomInteractionManager.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Input.hpp"
#include "fstream"

Room::~Room()
{
	// 析构函数 - 确保正确清理资源
	m_Characters.clear();
	m_RoomObjects.clear();
}

void Room::Start(const std::shared_ptr<Character> &player)
{
	// 更新缓存数据
	m_Player = player;
	UpdateCachedReferences();

	AddManager(ManagerTypes::ROOMCOLLISION, m_CollisionManager);
	AddManager(ManagerTypes::ROOMINTERACTIONMANAGER, m_InteractionManager);
	AddManager(ManagerTypes::TRACKING, m_TrackingManager);

	m_InteractionManager->SetPlayer(player);

	// 加载房间数据 TODO:要改 可能是读取head.json然后选择房间
	LoadFromJSON();
}

void Room::Update()
{
	// TODO: 房間管理員都交給Camera
	for (auto &[type, manager] : m_Managers)
	{
		manager->Update();
	}

	// 自動互動
	m_InteractionManager->UpdateAutoInteractions();

	// 玩家主動互動
	if (Util::Input::IsKeyDown(Util::Keycode::F))
	{
		m_InteractionManager->TryInteractWithClosest();
	}
}

void Room::PlayerEnter(const std::shared_ptr<Character> &player)
{
	if (!player || HasPlayer(player))
		return;

	m_Players.push_back(player);
	RegisterObjectToSceneAndManager(std::static_pointer_cast<nGameObject>(player));
	UpdateAllCharactersView();
	OnPlayerEnter(player);

	LOG_DEBUG("Player entered room at ({}, {})", m_RoomSpaceInfo.m_WorldCoord.x, m_RoomSpaceInfo.m_WorldCoord.y);
}

void Room::PlayerExit(const std::shared_ptr<Character> &player)
{
	if (!player)
		return;

	auto it = std::find(m_Players.begin(), m_Players.end(), player);
	if (it != m_Players.end())
	{
		m_Players.erase(it);
		UnRegisterObjectToSceneAndManager(std::static_pointer_cast<nGameObject>(player));
		UpdateAllCharactersView();
		OnPlayerExit(player);

		LOG_DEBUG("Player exited room");
	}
}

void Room::RemovePlayerFromList(const std::shared_ptr<Character> &player)
{
	if (!player)
		return;

	auto it = std::find(m_Players.begin(), m_Players.end(), player);
	if (it != m_Players.end())
	{
		m_Players.erase(it);
		UpdateAllCharactersView();
		// 不調用 UnRegisterObjectToSceneAndManager，保持玩家在場景中
		// 不調用 OnPlayerExit，避免觸發其他邏輯

		LOG_DEBUG("Player removed from room list (but kept in scene)");
	}
}

bool Room::HasPlayer(const std::shared_ptr<Character> &player) const
{
	return std::find(m_Players.begin(), m_Players.end(), player) != m_Players.end();
}

void Room::SpawnEntity(const std::shared_ptr<Character> &entity, EntityCategory category)
{
	if (!entity || HasEntity(entity))
		return;

	AddToContainer(entity, category);
	RegisterObjectToSceneAndManager(std::static_pointer_cast<nGameObject>(entity));
	UpdateAllCharactersView();
	OnEntitySpawned(entity, category);
}

void Room::RemoveEntity(const std::shared_ptr<Character> &entity)
{
	if (!entity)
		return;

	EntityCategory category = GetEntityCategory(entity);
	RemoveFromContainer(entity);
	UnRegisterObjectToSceneAndManager(std::static_pointer_cast<nGameObject>(entity));
	UpdateAllCharactersView();
	OnEntityRemoved(entity, category);

	LOG_DEBUG("Entity removed from room");
}

bool Room::HasEntity(const std::shared_ptr<Character> &entity) const
{
	return std::find(m_NPCs.begin(), m_NPCs.end(), entity) != m_NPCs.end() ||
		std::find(m_Enemies.begin(), m_Enemies.end(), entity) != m_Enemies.end() ||
		std::find(m_OtherEntities.begin(), m_OtherEntities.end(), entity) != m_OtherEntities.end();
}

std::vector<std::shared_ptr<Character>> Room::GetAllCharacters() const
{
	return m_AllCharacters; // 返回統一視圖
}

void Room::CharacterEnter(const std::shared_ptr<Character> &character)
{
	if (character && !HasCharacter(character))
	{
		// 加入生物组
		m_Characters.push_back(character);
		// 注册渲染器、相机、管理员
		RegisterObjectToSceneAndManager(std::static_pointer_cast<nGameObject>(character));

		// 同時更新新的分離容器（智能判斷類型）
		EntityCategory category = GetEntityCategory(character);
		AddToContainer(character, category);
		UpdateAllCharactersView();

		// 触发进入反应 TODO:进入事件应该是Dungeon里面的房间大小
		OnCharacterEnter(character);
	}
}

void Room::CharacterExit(const std::shared_ptr<Character> &character)
{
	if (!character)
		return;
	if (const auto it = std::find(m_Characters.begin(), m_Characters.end(), character); it != m_Characters.end())
	{
		m_Characters.erase(it);
		UnRegisterObjectToSceneAndManager(std::static_pointer_cast<nGameObject>(character));

		// 同時從新的分離容器中移除
		RemoveFromContainer(character);
		UpdateAllCharactersView();

		OnCharacterExit(character);
	}
}

bool Room::HasCharacter(const std::shared_ptr<Character> &character) const
{
	return std::find(m_Characters.begin(), m_Characters.end(), character) != m_Characters.end();
}

void Room::AddRoomObject(const std::shared_ptr<nGameObject> &object)
{
	if (object)
	{
		m_RoomObjects.push_back(object);
		RegisterObjectToSceneAndManager(object); // 一键加入渲染器、相机和管理员
	}
}

void Room::RemoveRoomObject(const std::shared_ptr<nGameObject> &object)
{
	if (object)
	{
		UnRegisterObjectToSceneAndManager(object); // 一键从渲染器、相机和管理员移除

		// 从列表移除
		m_RoomObjects.erase(std::remove(m_RoomObjects.begin(), m_RoomObjects.end(), object), m_RoomObjects.end());
	}
}

nlohmann::json Room::ReadJsonFile(const std::string &filePath) const
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		LOG_DEBUG("Error: Unable to open file: {}", filePath);
		return nlohmann::json();
	}

	nlohmann::json jsonData;
	file >> jsonData;
	return jsonData;
}

void Room::InitializeRoomObjects(const nlohmann::json &jsonData)
{
	m_RoomSpaceInfo.m_TileSize =
		glm::vec2(jsonData.at("tile_width").get<float>(), jsonData.at("tile_height").get<float>());
	m_RoomSpaceInfo.m_RoomRegion =
		glm::vec2(jsonData.at("room_region_x").get<float>(), jsonData.at("room_region_y").get<float>());
	m_RoomSpaceInfo.m_RoomSize =
		glm::vec2(jsonData.at("room_size_x").get<float>(), jsonData.at("room_size_y").get<float>());

	for (const auto &elem : jsonData["roomObject"])
	{
		auto roomObject =
			m_Factory.lock()->CreateRoomObject(elem.at("ID").get<std::string>(), elem.at("Class").get<std::string>());
		if (roomObject)
		{
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
	if (!object)
		return;
	if (!object->IsRegisteredToScene())
	{
		if (const auto scene = SceneManager::GetInstance().GetCurrentScene().lock())
		{
			scene->GetPendingObjects().emplace_back(object);
			object->SetRegisteredToScene(true);
		}
	}

	RegisterCollisionManager(object);
	RegisterInteractionManager(object);
	RegisterTrackingManager(object);
}

void Room::RegisterCollisionManager(const std::shared_ptr<nGameObject> &object) const
{
	if (const auto collComp = object->GetComponent<CollisionComponent>(ComponentType::COLLISION))
	{
		m_CollisionManager->RegisterNGameObject(object);
	}
}

void Room::RegisterInteractionManager(const std::shared_ptr<nGameObject> &object) const
{
	if (const auto interactComp = object->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
	{
		m_InteractionManager->RegisterInteractable(object);
		if (const std::shared_ptr<nGameObject> &promptObj = interactComp->GetPromptObject())
		{
			if (const auto scene = SceneManager::GetInstance().GetCurrentScene().lock())
			{
				scene->GetPendingObjects().emplace_back(promptObj);
			}
		}
		if (const std::shared_ptr<nGameObject> &promptUI = interactComp->GetPromptUI())
		{
			if (const auto scene = SceneManager::GetInstance().GetCurrentScene().lock())
			{
				scene->GetPendingObjects().emplace_back(promptUI);
			}
		}
	}
}

void Room::RegisterTrackingManager(const std::shared_ptr<nGameObject> &object) const
{
	if (object->GetZIndexType() < ZIndexType::ATTACK)
		return;
	const auto renderer = m_CachedRenderer.lock();
	const auto camera = m_CachedCamera.lock();
	if (const auto collComp = object->GetComponent<CollisionComponent>(ComponentType::COLLISION))
	{
		switch (collComp->GetCollisionLayer())
		{
		case CollisionLayers_Terrain:
			m_TrackingManager->AddTerrainObject(object);
			break;
		case CollisionLayers_Enemy:
			m_TrackingManager->AddEnemy(std::dynamic_pointer_cast<Character>(object));
			break;
		case CollisionLayers_Player:
			m_TrackingManager->SetPlayer(std::dynamic_pointer_cast<Character>(object));
			break;
		default:
			break;
		}
	}
}

void Room::UnRegisterObjectToSceneAndManager(const std::shared_ptr<nGameObject> &object) const
{
	if (!object)
		return;

	// === 第一步：從場景中移除物件 ===
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		const auto renderer = scene->GetRoot().lock();
		const auto camera = scene->GetCamera().lock();

		if (renderer)
			renderer->RemoveChild(object);
		if (camera)
			camera->MarkForRemoval(object);
	}

	// === 第二步：從各個管理器中反註冊物件 ===
	UnregisterCollisionManager(object);
	UnregisterInteractionManager(object);
	UnregisterTrackingManager(object);
}

void Room::UnregisterCollisionManager(const std::shared_ptr<nGameObject> &object) const
{
	if (const auto collComp = object->GetComponent<CollisionComponent>(ComponentType::COLLISION))
	{
		m_CollisionManager->UnregisterNGameObject(object);

		// 碰撞箱可視化功能已移除
		/*
		if (const std::shared_ptr<nGameObject> &colliderVisible = collComp->GetVisibleBox())
		{
			const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
			if (scene)
			{
				const auto renderer = scene->GetRoot().lock();
				const auto camera = scene->GetCamera().lock();

				if (renderer)
					renderer->RemoveChild(colliderVisible);
				if (camera)
					camera->MarkForRemoval(colliderVisible);
			}
		}
		*/
	}
}

void Room::UnregisterInteractionManager(const std::shared_ptr<nGameObject> &object) const
{
	if (const auto interactComp = object->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
	{
		m_InteractionManager->UnregisterInteractable(object);

		if (const std::shared_ptr<nGameObject> &promptObj = interactComp->GetPromptObject())
		{
			const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
			if (scene)
			{
				const auto renderer = scene->GetRoot().lock();
				const auto camera = scene->GetCamera().lock();

				if (renderer)
					renderer->RemoveChild(promptObj);
				if (camera)
					camera->MarkForRemoval(promptObj);
			}
		}
		if (const std::shared_ptr<nGameObject> &promptUI = interactComp->GetPromptUI())
		{
			const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
			if (scene)
			{
				const auto renderer = scene->GetRoot().lock();
				const auto camera = scene->GetCamera().lock();

				if (renderer)
					renderer->RemoveChild(promptUI);
				if (camera)
					camera->MarkForRemoval(promptUI);
			}
		}
	}
}

void Room::UnregisterTrackingManager(const std::shared_ptr<nGameObject> &object) const
{
	if (object->GetZIndexType() < ZIndexType::ATTACK)
		return;

	if (const auto collComp = object->GetComponent<CollisionComponent>(ComponentType::COLLISION))
	{
		switch (collComp->GetCollisionLayer())
		{
		case CollisionLayers_Terrain:
			m_TrackingManager->RemoveTerrainObject(object);
			break;
		case CollisionLayers_Enemy:
			if (auto character = std::dynamic_pointer_cast<Character>(object))
			{
				m_TrackingManager->RemoveEnemy(character);
			}
			break;
		case CollisionLayers_Player:
			// 玩家通常不需要從 TrackingManager 中移除，因為它是全局追蹤的
			// 但如果有需要，可以添加相應的方法
			break;
		default:
			break;
		}
	}
}

std::shared_ptr<nGameObject> Room::CreateChest(ChestType type) const
{
	auto factory = m_Factory.lock();
	if (!factory)
		return nullptr;

	// 使用工廠方法創建寶箱
	auto chest = factory->CreateChest(type, m_Player.lock());
	if (!chest)
		return nullptr;

	// 加入互動manager中
	m_InteractionManager->RegisterInteractable(chest);
	return chest;
}
std::shared_ptr<nGameObject> Room::CreateEnergyBall() const
{
	auto factory = m_Factory.lock();
	if (!factory)
		return nullptr;

	// 使用工廠方法創建寶箱
	auto energyBall = factory->CreateRoomObject("object_energyBall");
	if (!energyBall)
		return nullptr;

	// 加入互動manager中
	m_InteractionManager->RegisterInteractable(energyBall);
	return energyBall;
}

// 玩家位置檢測方法（基礎防漏洞機制）
bool Room::IsPlayerInsideRegion() const
{
	const auto player = m_Player.lock();
	if (!player)
		return false;

	const auto roomWorldCoord = m_RoomSpaceInfo.m_WorldCoord;
	const auto roomRegionInPixel = m_RoomSpaceInfo.m_RoomRegion * m_RoomSpaceInfo.m_TileSize;

	return (player->m_WorldCoord.x < roomWorldCoord.x + roomRegionInPixel.x / 2.0f) &&
		(player->m_WorldCoord.y < roomWorldCoord.y + roomRegionInPixel.y / 2.0f) &&
		(player->m_WorldCoord.x > roomWorldCoord.x - roomRegionInPixel.x / 2.0f) &&
		(player->m_WorldCoord.y > roomWorldCoord.y - roomRegionInPixel.y / 2.0f);
}

bool Room::IsPlayerInsideRoom(const float epsilon) const
{
	const auto player = m_Player.lock();
	if (!player)
		return false;

	const auto roomWorldCoord = m_RoomSpaceInfo.m_WorldCoord;
	const auto roomSizeInPixel = m_RoomSpaceInfo.m_RoomSize * m_RoomSpaceInfo.m_TileSize;

	return (player->m_WorldCoord.x < roomWorldCoord.x + (roomSizeInPixel.x / 2.0f - epsilon)) &&
		(player->m_WorldCoord.y < roomWorldCoord.y + (roomSizeInPixel.y / 2.0f - epsilon)) &&
		(player->m_WorldCoord.x > roomWorldCoord.x - (roomSizeInPixel.x / 2.0f - epsilon)) &&
		(player->m_WorldCoord.y > roomWorldCoord.y - (roomSizeInPixel.y / 2.0f - epsilon));
}

// === 私有輔助方法實現 ===

void Room::AddToContainer(const std::shared_ptr<Character> &character, EntityCategory category)
{
	switch (category)
	{
	case EntityCategory::PLAYER:
		if (std::find(m_Players.begin(), m_Players.end(), character) == m_Players.end())
		{
			m_Players.push_back(character);
		}
		break;
	case EntityCategory::ENEMY:
		if (std::find(m_Enemies.begin(), m_Enemies.end(), character) == m_Enemies.end())
		{
			m_Enemies.push_back(character);
		}
		break;
	case EntityCategory::NPC:
		if (std::find(m_NPCs.begin(), m_NPCs.end(), character) == m_NPCs.end())
		{
			m_NPCs.push_back(character);
		}
		break;
	case EntityCategory::OTHER:
	default:
		if (std::find(m_OtherEntities.begin(), m_OtherEntities.end(), character) == m_OtherEntities.end())
		{
			m_OtherEntities.push_back(character);
		}
		break;
	}
}

void Room::RemoveFromContainer(const std::shared_ptr<Character> &character)
{
	// 從所有容器中移除
	m_Players.erase(std::remove(m_Players.begin(), m_Players.end(), character), m_Players.end());
	m_Enemies.erase(std::remove(m_Enemies.begin(), m_Enemies.end(), character), m_Enemies.end());
	m_NPCs.erase(std::remove(m_NPCs.begin(), m_NPCs.end(), character), m_NPCs.end());
	m_OtherEntities.erase(std::remove(m_OtherEntities.begin(), m_OtherEntities.end(), character),
						  m_OtherEntities.end());
}

void Room::UpdateAllCharactersView()
{
	// 重建統一視圖
	m_AllCharacters.clear();

	// 添加所有類型的角色
	m_AllCharacters.insert(m_AllCharacters.end(), m_Players.begin(), m_Players.end());
	m_AllCharacters.insert(m_AllCharacters.end(), m_Enemies.begin(), m_Enemies.end());
	m_AllCharacters.insert(m_AllCharacters.end(), m_NPCs.begin(), m_NPCs.end());
	m_AllCharacters.insert(m_AllCharacters.end(), m_OtherEntities.begin(), m_OtherEntities.end());

	// 也更新舊的容器以保持兼容性
	m_Characters = m_AllCharacters;
}

EntityCategory Room::GetEntityCategory(const std::shared_ptr<Character> &character) const
{
	if (!character)
		return EntityCategory::OTHER;

	// 根據Character的類型來判斷分類
	switch (character->GetType())
	{
	case CharacterType::PLAYER:
		return EntityCategory::PLAYER;
	case CharacterType::ENEMY:
		return EntityCategory::ENEMY;
	case CharacterType::NPC:
		return EntityCategory::NPC;
	default:
		return EntityCategory::OTHER;
	}
}
