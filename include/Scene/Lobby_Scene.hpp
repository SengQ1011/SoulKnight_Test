//
// Created by QuzzS on 2025/3/4.
//

#ifndef LOBBY_SCENE_HPP
#define LOBBY_SCENE_HPP

#include "Scene/Scene.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Room/LobbyRoom.hpp"
#include "ObserveManager/TrackingManager.hpp"

#include "Util/GameObject.hpp"

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

	std::shared_ptr<TrackingManager> m_trackingManager = std::make_shared<TrackingManager>();

	std::shared_ptr<LobbyRoom> m_LobbyRoom;

	std::shared_ptr<Character> m_Player;
	std::shared_ptr<Character> m_Enemy;

	float m_MapHeight;

	std::shared_ptr<RoomObjectFactory> m_RoomObjectFactory;

	std::shared_ptr<Loader> m_Loader = std::make_shared<Loader>("Lobby");

	std::string m_ThemeName = "Lobby";//工廠和房間加載JSON用的 TODO:可能叫SceneManager傳入

	std::shared_ptr<BulletManager> m_BulletManager = std::make_shared<BulletManager>();
};

#endif //LOBBY_SCENE_HPP
