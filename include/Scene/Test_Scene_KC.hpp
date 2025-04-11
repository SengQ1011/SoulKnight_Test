//
// Created by QuzzS on 2025/3/4.
//

#ifndef TEST_SCENE_HPP
#define TEST_SCENE_HPP

#include "eventpp/callbacklist.h"
#include "eventpp/eventdispatcher.h"

#include "Camera.hpp"
#include "Components/MovementComponent.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Override/nGameObject.hpp"
#include "Room/LobbyRoom.hpp"
#include "Room/RoomCollisionManager.hpp"
#include "Scene/Scene.hpp"
#include "Util/Renderer.hpp"

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

	// 更新逻辑的辅助方法
	void HandlePlayerControls();

	std::shared_ptr<LobbyRoom> m_LobbyRoom;
	std::shared_ptr<Character> m_Player;
	float m_MapHeight;

	std::shared_ptr<RoomObjectFactory> m_RoomObjectFactory;

	std::shared_ptr<Loader> m_Loader = std::make_shared<Loader>("Lobby");

	std::string m_ThemeName = "Lobby";//工廠和房間加載JSON用的 TODO:可能叫SceneManager傳入

	// std::shared_ptr<RoomObject> m_Portal = std::make_shared<RoomObject>();
};

#endif //TEST_SCENE_HPP
