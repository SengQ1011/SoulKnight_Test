//
// Created by QuzzS on 2025/2/23.
//

#ifndef MENU_SCENE_HPP
#define MENU_SCENE_HPP

#include "App.hpp"
#include "Scene.hpp"

#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"

class MainMenuScene : public Scene
{
public:
	MainMenuScene() = default;
	~MainMenuScene() override = default;

	void Start() override;
	void Input() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

private:
	std::shared_ptr<Util::GameObject> m_Background = std::make_shared<Util::GameObject>();

	Util::Renderer m_Root;
};

#endif //MENU_SCENE_HPP
