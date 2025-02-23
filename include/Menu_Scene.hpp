//
// Created by QuzzS on 2025/2/23.
//

#ifndef MENU_SCENE_HPP
#define MENU_SCENE_HPP

#include "App.hpp"
#include "Scene.hpp"

class MenuScene : public Scene
{
public:
	MenuScene() = default;
	~MenuScene() override = default;

	 void Start() override;
	 void Update() override;
	 void Exit() override;
	 SceneType Change() override;

private:
};

#endif //MENU_SCENE_HPP
