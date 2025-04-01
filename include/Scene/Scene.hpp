//
// Created by QuzzS on 2025/3/4.
//

#ifndef SCENE_HPP
#define SCENE_HPP

#include "Camera.hpp"
#include "EnumTypes.hpp"
#include "Util/Renderer.hpp"
#include "Util/Time.hpp"

class Camera;
// TODO
enum class StageTheme
{
	Null,
	IcePlains,
};

struct SceneData
{
	//PlayerData
	//WeaponData
	//DungeonData
	int KillCount = 0;
	int StageCount = 0;
	int Point = 0;
	Util::ms_t DungeonStartTime = 0;
	StageTheme StageTheme = StageTheme::IcePlains;
};

class Scene {
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

	Scene() = default;
	virtual ~Scene() = default;

	virtual std::shared_ptr<SceneData> Upload()
	{
		return m_SceneData;
	};
	virtual void Download(const std::shared_ptr<SceneData>& data)
	{
		m_SceneData = data;
	};

	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;
	virtual SceneType Change() = 0; // 換場景設置

	std::weak_ptr<Util::Renderer> GetRoot() {return m_Root;}
	std::weak_ptr<Camera> GetCamera() {return m_Camera;}

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

protected:
	std::shared_ptr<SceneData> m_SceneData = nullptr;
	std::shared_ptr<Util::Renderer> m_Root = std::make_shared<Util::Renderer>();
	std::shared_ptr<Camera> m_Camera = std::make_shared<Camera>();
	std::unordered_map<ManagerTypes, std::shared_ptr<void>> m_Managers;			// 存儲各種 Manager
};

#endif //SCENE_HPP
