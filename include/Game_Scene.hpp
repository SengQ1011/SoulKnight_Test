//
// Created by QuzzS on 2025/2/23.
//

#ifndef GAME_SCENE_HPP
#define GAME_SCENE_HPP

#include "App.hpp"
#include "Scene.hpp"

class GameScene : public Scene
{
public:
	GameScene() = default;
	~GameScene() override = default;

	void Start() override;
	void Input() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

private:
};

#endif //GAME_SCENE_HPP
