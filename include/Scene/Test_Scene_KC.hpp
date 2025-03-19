//
// Created by QuzzS on 2025/3/4.
//

#ifndef TEST_SCENE_HPP
#define TEST_SCENE_HPP

#include "Camera.hpp"
#include "Components/MovementComponent.hpp"
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
	std::shared_ptr<nGameObject> m_Floor = std::make_shared<nGameObject>();
	std::shared_ptr<nGameObject> m_Wall = std::make_shared<nGameObject>();
	std::shared_ptr<MovementComponent> m_MoveComp;
	std::shared_ptr<RoomCollisionManager> m_RoomCollisionManager = std::make_shared<RoomCollisionManager>();

	std::shared_ptr<nGameObject> m_Character = std::make_shared<nGameObject>();
	std::shared_ptr<nGameObject> m_Enemy = std::make_shared<nGameObject>();
	std::shared_ptr<nGameObject> m_Weapon = std::make_shared<nGameObject>();
	Camera m_Camera;
	Beacon m_Beacon;

	Util::Renderer m_Root;
};

#endif //TEST_SCENE_HPP
