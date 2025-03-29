//
// Created by QuzzS on 2025/3/4.
//

#ifndef SCENE_HPP
#define SCENE_HPP

#include "Camera.hpp"
#include "Util/Renderer.hpp"
#include "Util/Time.hpp"

//TODO
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
	virtual void Input() = 0; //專門處理玩家輸入
	virtual void Update() = 0;
	virtual void Exit() = 0;
	virtual SceneType Change() = 0; // 換場景設置

	std::weak_ptr<Util::Renderer> GetRoot() {return m_Root;}
	std::weak_ptr<Camera> GetCamera() {return m_Camera;}

protected:
	std::shared_ptr<SceneData> m_SceneData = nullptr;
	std::shared_ptr<Util::Renderer> m_Root = std::make_shared<Util::Renderer>();
	std::shared_ptr<Camera> m_Camera = std::make_shared<Camera>();
};

#endif //SCENE_HPP
