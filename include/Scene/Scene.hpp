//
// Created by QuzzS on 2025/3/4.
//

#ifndef SCENE_HPP
#define SCENE_HPP

#include "Util/Time.hpp"

class Scene {
public:

	enum class SceneType
	{
		Null,
		Menu,
		Test_KC,
		Test_JX,
		Lobby,
		Game,
	};

	Scene() = default;
	virtual ~Scene() = default;

	virtual void Start() = 0;
	virtual void Input() = 0; //專門處理玩家輸入 TODO:需要一個内部狀態變量 - 將Input的data傳去Update
	virtual void Update() = 0;
	virtual void Exit() = 0;
	virtual SceneType Change() = 0; // 換場景設置

protected:
};

#endif //SCENE_HPP
