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
#include "Room/RoomCollisionManager.hpp"
#include "Scene/Scene.hpp"
#include "Util/Renderer.hpp"

class TestScene_KC : public Scene
{
public:
	TestScene_KC() = default;
	~TestScene_KC() override = default;

	void Start() override;
	void Input() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
	std::vector<std::shared_ptr<nGameObject>> m_RoomObject;
	std::vector<std::shared_ptr<nGameObject>> m_WallCollider;
	std::shared_ptr<RoomObjectFactory> m_Factory = std::make_shared<RoomObjectFactory>();
	std::shared_ptr<RoomCollisionManager> m_RoomCollisionManager = std::make_shared<RoomCollisionManager>();
	std::shared_ptr<nGameObject> m_Player = std::make_shared<nGameObject>();

	float roomHeight = 0;
	std::shared_ptr<Camera> m_Camera = std::make_shared<Camera>();
	Util::Renderer m_Root;
};

#endif //TEST_SCENE_HPP
