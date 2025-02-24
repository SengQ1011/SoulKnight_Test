//
// Created by QuzzS on 2025/2/23.
//

#ifndef SCENE_H
#define SCENE_H

class Scene {
public:

	enum class SceneType
	{
		Null,
		Menu,
		Game,
	};

	Scene() = default;
	virtual ~Scene() = default;

	virtual void Start() = 0;
	virtual void Input() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;
	virtual SceneType Change() = 0; // 換場景設置

private:
};

#endif //SCENE_H
