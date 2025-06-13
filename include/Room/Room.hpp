//
// Created by QuzzS on 2025/3/7.
//

#ifndef ROOM_HPP
#define ROOM_HPP

#include "EnumTypes.hpp"
#include "Util/Logger.hpp"
#include "json.hpp"

#include "ObserveManager/TrackingManager.hpp"
#include "RoomCollisionManager.hpp"
#include "RoomInteractionManager.hpp"


class Scene;
class Camera;
class nGameObject;
class Character;
class Loader;
class RoomObjectFactory;
class RoomInteractionManager;
class RoomCollisionManager;
namespace Util
{
	class Renderer;
}

struct RoomSpaceInfo
{
	glm::vec2 m_WorldCoord = glm::vec2(0, 0); // 在世界中的位置
	glm::vec2 m_RoomRegion = glm::vec2(0, 0); // 房间區域
	glm::vec2 m_TileSize = glm::vec2(0, 0); // 瓦片尺寸
	glm::vec2 m_RoomSize = glm::vec2(0, 0); // 不包括通道的房間大小
};

/// @brief: 一定要設置m_ObjectPositionPath
class Room
{
public:
	explicit Room(const glm::vec2 worldCoord, const std::shared_ptr<Loader> &loader,
				  const std::shared_ptr<RoomObjectFactory> &room_object_factory) :
		m_Loader(loader), m_Factory(room_object_factory)
	{
		m_RoomSpaceInfo.m_WorldCoord = worldCoord;
	}
	virtual ~Room();

	// 核心方法
	virtual void Start(const std::shared_ptr<Character> &player); // 房间初始化
	virtual void Update(); // 更新房间内所有对象

	template <typename T>
	std::shared_ptr<T> GetManager(const ManagerTypes managerName)
	{
		auto it = m_Managers.find(managerName);
		if (it != m_Managers.end())
		{
			return std::static_pointer_cast<T>(it->second);
		}
		return nullptr; // 若找不到指定類型的 Manager 返回 nullptr
	}

	void AddManager(const ManagerTypes managerName, std::shared_ptr<IManager> manager)
	{
		m_Managers[managerName] = manager;
		// LOG_DEBUG("Successfully added new Manager");
	}

	// === 新的分離角色管理方法 ===

	// 外部角色管理（玩家等從外部進入房間的角色）
	virtual void PlayerEnter(const std::shared_ptr<Character> &player);
	virtual void PlayerExit(const std::shared_ptr<Character> &player);
	void RemovePlayerFromList(const std::shared_ptr<Character> &player); // 只從列表移除，不從場景移除
	[[nodiscard]] bool HasPlayer(const std::shared_ptr<Character> &player) const;
	[[nodiscard]] const std::vector<std::shared_ptr<Character>> &GetPlayers() const { return m_Players; }

	// 內部實體管理（房間內生成的NPC、怪物等）
	virtual void SpawnEntity(const std::shared_ptr<Character> &entity, EntityCategory category = EntityCategory::OTHER);
	virtual void RemoveEntity(const std::shared_ptr<Character> &entity);
	[[nodiscard]] bool HasEntity(const std::shared_ptr<Character> &entity) const;
	[[nodiscard]] const std::vector<std::shared_ptr<Character>> &GetNPCs() const { return m_NPCs; }
	[[nodiscard]] const std::vector<std::shared_ptr<Character>> &GetEnemies() const { return m_Enemies; }
	[[nodiscard]] const std::vector<std::shared_ptr<Character>> &GetOtherEntities() const { return m_OtherEntities; }

	// 獲取所有角色（統一視圖）
	[[nodiscard]] std::vector<std::shared_ptr<Character>> GetAllCharacters() const;

	// === 舊的通用角色管理方法（向後兼容） ===
	virtual void CharacterEnter(const std::shared_ptr<Character> &character);
	virtual void CharacterExit(const std::shared_ptr<Character> &character);
	[[nodiscard]] bool HasCharacter(const std::shared_ptr<Character> &character) const;
	[[nodiscard]] const std::vector<std::shared_ptr<Character>> &GetCharacters() const { return m_AllCharacters; }

	// 房间对象管理
	void AddRoomObject(const std::shared_ptr<nGameObject> &object);
	void RemoveRoomObject(const std::shared_ptr<nGameObject> &object);
	[[nodiscard]] const std::vector<std::shared_ptr<nGameObject>> &GetRoomObjects() const { return m_RoomObjects; }

	// 碰撞体管理
	[[nodiscard]] std::shared_ptr<RoomCollisionManager> GetCollisionManager() const { return m_CollisionManager; }
	[[nodiscard]] std::shared_ptr<RoomInteractionManager> GetInteractionManager() const { return m_InteractionManager; }
	[[nodiscard]] std::shared_ptr<TrackingManager> GetTrackingManager() const { return m_TrackingManager; }

	// Getter/Setter
	[[nodiscard]] const RoomSpaceInfo &GetRoomSpaceInfo() const { return m_RoomSpaceInfo; }

	void SetWorldCoord(const glm::vec2 &worldCoord) { m_RoomSpaceInfo.m_WorldCoord = worldCoord; }
	void SetRoomRegion(const glm::vec2 &region) { m_RoomSpaceInfo.m_RoomRegion = region; }
	void SetTileSize(const glm::vec2 &tileSize) { m_RoomSpaceInfo.m_TileSize = tileSize; }
	void SetRoomSize(const glm::vec2 &size) { m_RoomSpaceInfo.m_RoomSize = size; }

	void SetPlayer(const std::shared_ptr<Character> &player) { m_Player = player; }

	// 加载JSON配置
	virtual void LoadFromJSON() = 0;

	// 玩家位置檢測（基礎防漏洞機制）
	[[nodiscard]] virtual bool IsPlayerInsideRegion() const;
	[[nodiscard]] virtual bool IsPlayerInsideRoom(float epsilon = 16.0f) const;

	// 創建物品
	std::shared_ptr<nGameObject> CreateChest(ChestType type) const;
	std::shared_ptr<nGameObject> CreateEnergyBall() const;

protected:
	// 房间属性
	RoomSpaceInfo m_RoomSpaceInfo;

	std::unordered_map<ManagerTypes, std::shared_ptr<IManager>> m_Managers; // 存儲各種 Manager

	// 房间内对象
	std::vector<std::shared_ptr<nGameObject>> m_RoomObjects; // 房间固定物体

	// === 新的分離角色容器 ===
	std::vector<std::shared_ptr<Character>> m_Players; // 外部進入的玩家
	std::vector<std::shared_ptr<Character>> m_NPCs; // 房間內的NPC
	std::vector<std::shared_ptr<Character>> m_Enemies; // 房間內的敵人
	std::vector<std::shared_ptr<Character>> m_OtherEntities; // 其他實體

	// === 舊的統一容器（向後兼容） ===
	std::vector<std::shared_ptr<Character>> m_Characters; // 舊版本使用的容器
	std::vector<std::shared_ptr<Character>> m_AllCharacters; // 所有角色的統一視圖

	/**
	 * @brief Room的Manager成員
	 * @note 未來所有局部Manager都在這裏建構
	 */
	std::shared_ptr<RoomCollisionManager> m_CollisionManager = std::make_shared<RoomCollisionManager>();
	std::shared_ptr<RoomInteractionManager> m_InteractionManager = std::make_shared<RoomInteractionManager>();
	std::shared_ptr<TrackingManager> m_TrackingManager = std::make_shared<TrackingManager>();
	/// @todo 未來可期

	// 緩存引用
	std::weak_ptr<Loader> m_Loader; // 專門用來讀取Json檔案
	// 工廠
	std::weak_ptr<RoomObjectFactory> m_Factory;
	std::weak_ptr<Character> m_Player;
	std::weak_ptr<Scene> m_CachedCurrentScene;
	std::weak_ptr<Camera> m_CachedCamera;
	std::weak_ptr<Util::Renderer> m_CachedRenderer;

	// === 新的事件處理方法 ===
	virtual void OnPlayerEnter(const std::shared_ptr<Character> &player) {}
	virtual void OnPlayerExit(const std::shared_ptr<Character> &player) {}
	virtual void OnEntitySpawned(const std::shared_ptr<Character> &entity, EntityCategory category) {}
	virtual void OnEntityRemoved(const std::shared_ptr<Character> &entity, EntityCategory category) {}

	// === 舊的事件處理方法（向後兼容） ===
	virtual void OnCharacterEnter(const std::shared_ptr<Character> &character) {}
	virtual void OnCharacterExit(const std::shared_ptr<Character> &character) {}

	// 輔助方法
	void UpdateCachedReferences();
	void RegisterObjectToSceneAndManager(const std::shared_ptr<nGameObject> &object) const;
	void UnRegisterObjectToSceneAndManager(const std::shared_ptr<nGameObject> &object) const;

	/**
	 * @defgroup LoadFromJSON 輔助方法
	 * @brief JSON 加載相關函數集合
	 * 功能流程：LoadFromJSON = LoadLobbyObjectPosition + InitializeRoomObjects;
	 * 其中 LoadLobbyObjectPosition 调用 ReadJsonFile
	 * @link Room::LoadFromJSON CPP實作位置 @endlink
	 */
	[[nodiscard]] nlohmann::json ReadJsonFile(const std::string &filePath) const;
	void InitializeRoomObjects(const nlohmann::json &jsonData);
	/// @}

	// 嘗試注冊到管理員
	void RegisterCollisionManager(const std::shared_ptr<nGameObject> &object) const;
	void RegisterInteractionManager(const std::shared_ptr<nGameObject> &object) const;
	void RegisterTrackingManager(const std::shared_ptr<nGameObject> &object) const;

	// 從管理員中反註冊
	void UnregisterCollisionManager(const std::shared_ptr<nGameObject> &object) const;
	void UnregisterInteractionManager(const std::shared_ptr<nGameObject> &object) const;
	void UnregisterTrackingManager(const std::shared_ptr<nGameObject> &object) const;

private:
	// === 內部輔助方法 ===
	void AddToContainer(const std::shared_ptr<Character> &character, EntityCategory category);
	void RemoveFromContainer(const std::shared_ptr<Character> &character);
	void UpdateAllCharactersView(); // 更新統一視圖
	EntityCategory GetEntityCategory(const std::shared_ptr<Character> &character) const;
};

#endif // ROOM_HPP
