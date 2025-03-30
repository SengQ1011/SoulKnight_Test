//
// Created by QuzzS on 2025/3/4.
//

#ifndef DUNGEON_HPP
#define DUNGEON_HPP

#include "Scene/Scene.hpp"

class DungeonScene : public Scene
{
public:
	DungeonScene() = default;
	~DungeonScene() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
};

#endif //DUNGEON_HPP
