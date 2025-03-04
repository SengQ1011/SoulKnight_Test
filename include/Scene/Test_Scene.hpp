//
// Created by QuzzS on 2025/3/4.
//

#ifndef TEST_SCENE_HPP
#define TEST_SCENE_HPP


#include "App.hpp"
#include "Scene/Scene.hpp"
#include "Camera.hpp"
#include "Override/nGameObject.hpp"

class TestScene : public Scene
{
public:
	TestScene() = default;
	~TestScene() override = default;

	void Start() override;
	void Input() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
	std::shared_ptr<nGameObject> m_Background = std::make_shared<nGameObject>();

	std::shared_ptr<nGameObject> m_Character = std::make_shared<nGameObject>();
	std::shared_ptr<nGameObject> m_Enemy = std::make_shared<nGameObject>();
	std::shared_ptr<nGameObject> m_Weapon = std::make_shared<nGameObject>();
	Camera m_Camera;
	Beacon m_Beacon;

	Util::Renderer m_Root;
};

#endif //TEST_SCENE_HPP
