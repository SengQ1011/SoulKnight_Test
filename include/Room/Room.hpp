//
// Created by QuzzS on 2025/3/7.
//

#ifndef ROOM_HPP
#define ROOM_HPP

#include "EnumTypes.hpp"
#include "Util/Logger.hpp"
#include "json.hpp"
#include "pch.hpp"

class Scene;
class Camera;
class nGameObject;
class Character;
class Loader;
class RoomObjectFactory;
class RoomInteractionManager;
class RoomCollisionManager;
namespace Util {class Renderer;}

struct RoomSpaceInfo
{
	glm::vec2 m_WorldCoord = glm::vec2(0, 0);  // 在世界中的位置
	glm::vec2 m_RoomRegion = glm::vec2(0, 0);  // 房间區域
	glm::vec2 m_TileSize = glm::vec2(0, 0);    // 瓦片尺寸
	glm::vec2 m_RoomSize = glm::vec2(0, 0);	   // 不包括通道的房間大小
};

/// @brief: 一定要設置m_ObjectPositionPath
class Room {
public:
	explicit Room(const glm::vec2 worldCoord, const std::shared_ptr<Loader> &loader,
				  const std::shared_ptr<RoomObjectFactory> &room_object_factory) :
		m_Loader(loader), m_Factory(room_object_factory)
	{ m_RoomSpaceInfo.m_WorldCoord = worldCoord; }
	virtual ~Room();

    // 核心方法
    virtual void Start(const std::shared_ptr<Character>& player); // 房间初始化
    virtual void Update(); // 更新房间内所有对象

	template <typename T>
	std::shared_ptr<T> GetManager(const ManagerTypes managerName) {
		auto it = m_Managers.find(managerName);
		if (it != m_Managers.end()) {
			return std::static_pointer_cast<T>(it->second);
		}
		return nullptr;  // 若找不到指定類型的 Manager 返回 nullptr
	}

	void AddManager(const ManagerTypes managerName, std::shared_ptr<void> manager) {
		m_Managers[managerName] = manager;
		LOG_DEBUG("Successfully added new Manager");
	}

    // 角色管理方法
    virtual void CharacterEnter(const std::shared_ptr<Character>& character);
    virtual void CharacterExit(const std::shared_ptr<Character>& character);
    [[nodiscard]] bool HasCharacter(const std::shared_ptr<Character>& character) const;
    [[nodiscard]] const std::vector<std::shared_ptr<Character>>& GetCharacters() const { return m_Characters; }

    // 房间对象管理
    void AddRoomObject(const std::shared_ptr<nGameObject>& object);
    void RemoveRoomObject(const std::shared_ptr<nGameObject>& object);
    [[nodiscard]] const std::vector<std::shared_ptr<nGameObject>>& GetRoomObjects() const { return m_RoomObjects; }

    // 碰撞体管理
    [[nodiscard]] std::shared_ptr<RoomCollisionManager> GetCollisionManager() const { return m_CollisionManager; }
    [[nodiscard]] std::shared_ptr<RoomInteractionManager> GetInteractionManager() const { return m_InteractionManager; }
    // [[nodiscard]] std::shared_ptr<BulletManager> GetBulletManager() const { return m_BulletManager; }
    // [[nodiscard]] std::shared_ptr<TrackingManager> GetTrackingManager() const { return m_TrackingManager; }

    // Getter/Setter
    [[nodiscard]] const RoomSpaceInfo& GetRoomSpaceInfo() const { return m_RoomSpaceInfo; }

    void SetWorldCoord(const glm::vec2& worldCoord) { m_RoomSpaceInfo.m_WorldCoord = worldCoord; }
    void SetRoomRegion(const glm::vec2& region) { m_RoomSpaceInfo.m_RoomRegion = region; }
    void SetTileSize(const glm::vec2& tileSize) { m_RoomSpaceInfo.m_TileSize = tileSize; }
	void SetRoomSize(const glm::vec2& size) { m_RoomSpaceInfo.m_RoomSize = size; }

	void SetPlayer(const std::shared_ptr<Character>& player) { m_Player = player; }

    // 加载JSON配置
    virtual void LoadFromJSON() = 0;

protected:
    // 房间属性
	RoomSpaceInfo m_RoomSpaceInfo;

	std::unordered_map<ManagerTypes, std::shared_ptr<void>> m_Managers;			// 存儲各種 Manager

    // 房间内对象
    std::vector<std::shared_ptr<nGameObject>> m_RoomObjects;	  // 房间固定物体
    std::vector<std::shared_ptr<Character>> m_Characters;         // 当前在房间内的角色

	/**
	 * @brief Room的Manager成員
	 * @note 未來所有局部Manager都在這裏建構
	 */
    std::shared_ptr<RoomCollisionManager> m_CollisionManager;
	std::shared_ptr<RoomInteractionManager> m_InteractionManager;
	/// @todo 未來可期

    // 緩存引用
	std::weak_ptr<Loader> m_Loader; // 專門用來讀取Json檔案
	// 工廠
	std::weak_ptr<RoomObjectFactory> m_Factory;
	std::weak_ptr<Character> m_Player;
	std::weak_ptr<Scene> m_CachedCurrentScene;
	std::weak_ptr<Camera> m_CachedCamera;
	std::weak_ptr<Util::Renderer> m_CachedRenderer;

    // 处理角色进入/离开房间时的事件 TODO:怎麽處理
    virtual void OnCharacterEnter(const std::shared_ptr<Character>& character) {}
    virtual void OnCharacterExit(const std::shared_ptr<Character>& character) {}

	//輔助方法
	void UpdateCachedReferences();
	void RegisterObjectToSceneAndManager(const std::shared_ptr<nGameObject>& object) const;
	void UnRegisterObjectToSceneAndManager(const std::shared_ptr<nGameObject>& object) const;

	/**
	 * @defgroup LoadFromJSON 輔助方法
	 * @brief JSON 加載相關函數集合
	 * 功能流程：LoadFromJSON = LoadLobbyObjectPosition + InitializeRoomObjects;
	 * 其中 LoadLobbyObjectPosition 调用 ReadJsonFile
	 * @link Room::LoadFromJSON CPP實作位置 @endlink
	 */
	[[nodiscard]] nlohmann::ordered_json ReadJsonFile(const std::string& filePath) const;
	void InitializeRoomObjects(const nlohmann::json& jsonData);
	/// @}

	//嘗試注冊到管理員
	void RegisterCollisionManger(const std::shared_ptr<nGameObject>& object) const;
	void RegisterInteractionManager(const std::shared_ptr<nGameObject>& object) const;
	void RegisterTrackingManager(const std::shared_ptr<nGameObject>& object) const;
};

#endif //ROOM_HPP