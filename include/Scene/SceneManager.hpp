//
// Created by QuzzS on 2025/3/6.
//

#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "Scene/Scene.hpp"
#include "Scene/MainMenu_Scene.hpp"
#include "Scene/Lobby_Scene.hpp"
#include "Scene/DungeonLoading_Scene.hpp"
#include "Scene/Dungeon_Scene.hpp"
#include "Scene/Complete_Scene.hpp"
#include "Scene/Result_Scene.hpp"
#include "Scene/Test_Scene_JX.hpp"
#include "Scene/Test_Scene_KC.hpp"

#include "pch.hpp" // IWYU pragma: export

class SceneManager {
public:
	//場景初始化用功能
	void ChangeCurrentScene();

	void Start();

	void Update() const; // 游戲循環 （場景切換 + 場景更新）

	void End(); // NOLINT(readability-convert-member-functions-to-static)

protected:
	static std::shared_ptr<Scene> CreateScene(Scene::SceneType type);

private:
	Scene::SceneType m_NextSceneType = Scene::SceneType::Null;
	std::shared_ptr<Scene> m_CurrentScene = nullptr;
	std::shared_ptr<Scene> m_PreLoadScene = nullptr;
	std::shared_ptr<SceneData> m_Data = std::make_shared<SceneData>();
	//TODO: Dungeon預加載指標
};


#endif //SCENEMANAGER_HPP
