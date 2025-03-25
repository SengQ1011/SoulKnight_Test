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
#include "Util/Time.hpp"
#include "Factory/CharacterFactory.hpp"

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
	void AddGameObjectToRoot(std::shared_ptr<Util::GameObject> gameObject) {
		m_Root.AddChild(gameObject);
	}

protected:
	CharacterFactory factory;
	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>();

	std::shared_ptr<Player> m_Player = factory.createPlayer(1);
	std::shared_ptr<nGameObject> m_Wall = std::make_shared<nGameObject>();
	//std::shared_ptr<Weapon> m_Weapon = std::make_shared<Weapon>(RESOURCE_DIR"/weapons_19.png", "Old_worn_pistol", 0, 1, 1,0,0,0,0,0);
	//std::shared_ptr<Enemy> m_Enemy = std::make_shared<Enemy>(RESOURCE_DIR"/Sprite/monster小怪/冰原/enemy31礦工/enemy31_0.png", 20, 1.0f, 10, nullptr, nullptr);
	std::shared_ptr<nGameObject> a;

	Camera m_Camera;

	Util::Renderer m_Root;
};

#endif //TEST_SCENE_JX_HPP
