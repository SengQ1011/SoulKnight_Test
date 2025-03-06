//
// Created by QuzzS on 2025/3/4.
//

#ifndef DUNGEONLOADING_HPP
#define DUNGEONLOADING_HPP

#include "Scene/Scene.hpp"

class DungeonLoadingScene : public Scene
{
public:
	DungeonLoadingScene() = default;
	~DungeonLoadingScene() override = default;

	void Start() override;
	void Input() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
};

#endif //DUNGEONLOADING_HPP
