//
// Created by QuzzS on 2025/3/4.
//

#ifndef TEST_SCENE_JX_HPP
#define TEST_SCENE_JX_HPP

#include "Camera.hpp"
#include "Override/nGameObject.hpp"
#include "Scene/Scene.hpp"
#include "Util/Renderer.hpp"
#include "Util/Time.hpp"
#include "Factory/CharacterFactory.hpp"
#include "Room/RoomCollisionManager.hpp"
#include "Factory/RoomObjectFactory.hpp"

class TestScene_JX : public Scene
{
public:
	TestScene_JX() = default;
	~TestScene_JX() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
	std::vector<std::shared_ptr<nGameObject>> m_RoomObject;
	std::vector<std::shared_ptr<nGameObject>> m_WallCollider;
	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>();
	std::shared_ptr<BulletManager> bulletManager = std::make_shared<BulletManager>();
	std::shared_ptr<RoomCollisionManager> m_RoomCollisionManager = std::make_shared<RoomCollisionManager>();

	std::shared_ptr<RoomObjectFactory> m_Factory = std::make_shared<RoomObjectFactory>();
	std::shared_ptr<Character> m_Player;
	std::shared_ptr<Character> m_Enemy;

	float roomHeight = 0;
};

#endif //TEST_SCENE_JX_HPP
