//
// Created by QuzzS on 2025/3/4.
//

#ifndef RESULT_SCENE_HPP
#define RESULT_SCENE_HPP

#include "Scene/Scene.hpp"

class ResultScene : public Scene
{
public:
	ResultScene() = default;
	~ResultScene() override = default;

	void Start() override;
	void Input() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
};

#endif //RESULT_SCENE_HPP
