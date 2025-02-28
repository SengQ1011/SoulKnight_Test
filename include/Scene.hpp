//
// Created by QuzzS on 2025/2/23.
//

#ifndef SCENE_H
#define SCENE_H

#include "Util/Time.hpp"

class Scene {
public:

	enum class SceneType
	{
		Null,
		Menu,
		Test,
		Lobby,
		Game,
	};

	Scene() = default;
	virtual ~Scene() = default;

	virtual void Start() = 0;
	virtual void Input() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;
	virtual SceneType Change() = 0; // 換場景設置

protected:
};

#endif //SCENE_H
