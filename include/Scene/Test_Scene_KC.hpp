//
// Created by QuzzS on 2025/3/4.
//

#ifndef TEST_SCENE_HPP
#define TEST_SCENE_HPP

#include "Attack/AttackManager.hpp"
#include "Loader.hpp"
#include "Room/DungeonRoom.hpp"
#include "Scene/Scene.hpp"


class DungeonMap;
class TestScene_KC : public Scene
{
public:
	TestScene_KC() = default;
	~TestScene_KC() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
	// 私有方法，按功能划分场景初始化流程
	void CreatePlayer();
	void SetupCamera() const;
	void InitializeSceneManagers();
	// std::vector<std::shared_ptr<DungeonRoom>> m_DungeonRooms = std::vector<std::shared_ptr<DungeonRoom>>(25,nullptr);
	std::shared_ptr<Character> m_Player;
	float m_MapHeight;

	std::shared_ptr<DungeonMap> m_Map;
	std::shared_ptr<RoomObjectFactory> m_RoomObjectFactory;

	std::string m_ThemeName = "IcePlains";//工廠和房間加載JSON用的 TODO:可能叫SceneManager傳入
	std::shared_ptr<Loader> m_Loader;

	//test
	std::vector<std::shared_ptr<nGameObject>> m_RoomObjectGroup;
	std::shared_ptr<AttackManager> m_AttackManager = std::make_shared<AttackManager>();
};

#endif //TEST_SCENE_HPP
