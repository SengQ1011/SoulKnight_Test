//
// Created by QuzzS on 2025/3/4.
//

#ifndef SCENE_HPP
#define SCENE_HPP

#include "Camera.hpp"
#include "Room/Room.hpp"
#include "Util/Renderer.hpp"
#include "Util/Time.hpp"

class RoomCollisionManager;
struct SaveData;
class Camera;
// TODO
enum class StageTheme
{
	Null,
	IcePlains,
};

class Scene
{
public:
	enum class SceneType
	{
		Null,
		Menu,
		Test_KC,
		Test_JX,
		Lobby,
		DungeonLoad,
		Dungeon,
		Complete,
		Result,
	};

	// Scene() = default;
	explicit Scene(const SceneType sceneType = SceneType::Null) : m_SceneType(sceneType) {}
	virtual ~Scene() = default;

	virtual void Upload();
	virtual void Download();

	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;
	virtual SceneType Change() = 0; // 換場景設置

	std::shared_ptr<SaveData> GetSaveData() { return m_SceneData; };
	std::weak_ptr<Util::Renderer> GetRoot() { return m_Root; }
	std::weak_ptr<Camera> GetCamera() { return m_Camera; }
	std::shared_ptr<Room> GetCurrentRoom() { return m_CurrentRoom; }
	std::vector<std::shared_ptr<nGameObject>> &GetPendingObjects() { return m_PendingObjects; }
	std::shared_ptr<RoomCollisionManager> GetCurrentCollisionManager()
	{
		std::shared_ptr<RoomCollisionManager> collisionManager;
		// 碰撞管理員可能在場景也可能在房間
		if (m_CurrentRoom)
		{
			return m_CurrentRoom->GetCollisionManager();
		}
		return GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION);
	}

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
		LOG_DEBUG("Successfully added new Manager");
	}

	bool IsChange() const { return m_IsChange; }
	void SetIsChange(const bool change) { m_IsChange = change; }

	SceneType GetSceneType() const { return m_SceneType; }

	void FlushPendingObjectsToRendererAndCamera();
	void SavePlayerInformation(std::shared_ptr<Character> player) const;

protected:
	bool m_IsChange = false;
	SceneType m_SceneType = SceneType::Null;
	std::shared_ptr<SaveData> m_SceneData = nullptr;
	std::shared_ptr<Room> m_CurrentRoom = nullptr;
	std::shared_ptr<Util::Renderer> m_Root = std::make_shared<Util::Renderer>();
	std::shared_ptr<Camera> m_Camera = std::make_shared<Camera>();
	std::vector<std::shared_ptr<nGameObject>> m_PendingObjects; // DungeonScene预加载暂存
	std::unordered_map<ManagerTypes, std::shared_ptr<IManager>> m_Managers; // 存儲各種 Manager
};

#endif // SCENE_HPP
