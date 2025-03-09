//
// Created by QuzzS on 2025/3/4.
//

#ifndef TEST_SCENE_JX_HPP
#define TEST_SCENE_JX_HPP

#include "Camera.hpp"
#include "Creature/Player.hpp"
#include "Creature/Enemy.hpp"
#include "Override/nGameObject.hpp"
#include "Scene/Scene.hpp"
#include "Util/Renderer.hpp"

class TestScene_JX : public Scene
{
public:
	TestScene_JX() = default;
	~TestScene_JX() override = default;

	void Start() override;
	void Input() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
	std::shared_ptr<nGameObject> m_Background = std::make_shared<nGameObject>();

	std::shared_ptr<Player> m_Player = std::make_shared<Player>(RESOURCE_DIR"/knight_0_0.png", 7, 2.0f, 5, nullptr, nullptr, 6, 180, 0.2f, 3, nullptr);
	std::shared_ptr<Enemy> m_Enemy = std::make_shared<Enemy>(RESOURCE_DIR"/Sprite/monster小怪/冰原/enemy31礦工", 20, 1.0f, 10, nullptr, nullptr);
	std::shared_ptr<Weapon> m_Weapon = std::make_shared<Weapon>(RESOURCE_DIR"/weapons_19.png", 10);
	Camera m_Camera;

	Util::Renderer m_Root;
};

#endif //TEST_SCENE_JX_HPP
