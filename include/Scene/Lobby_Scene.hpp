//
// Created by QuzzS on 2025/3/4.
//

#ifndef LOBBY_SCENE_HPP
#define LOBBY_SCENE_HPP

#include "Scene/Scene.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "ObserveManager/TrackingManager.hpp"
#include "Util/BGM.hpp"
#include "Attack/AttackManager.hpp"
#include "Loader.hpp"

class LobbyRoom;

class LobbyScene : public Scene
{
public:
	LobbyScene() = default;
	~LobbyScene() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
	void CreatePlayer();
	void CreateEnemy();
	void SetupCamera() const;
	void InitializeSceneManagers();

	// std::shared_ptr<TrackingManager> m_trackingManager = std::make_shared<TrackingManager>();

	std::shared_ptr<LobbyRoom> m_LobbyRoom;

	std::shared_ptr<Character> m_Player;
	std::vector<std::shared_ptr<Character>> m_Enemies;
	std::shared_ptr<Character> m_Enemy;

	float m_MapHeight = 0.0f;

	std::shared_ptr<RoomObjectFactory> m_RoomObjectFactory;

	std::shared_ptr<Loader> m_Loader = std::make_shared<Loader>("Lobby");
	std::shared_ptr<Util::BGM> m_BGM = std::make_shared<Util::BGM>(RESOURCE_DIR"/UI/bgm_openingLow.wav");

	std::string m_ThemeName = "Lobby";//工廠和房間加載JSON用的 TODO:可能叫SceneManager傳入
};

#endif //LOBBY_SCENE_HPP
