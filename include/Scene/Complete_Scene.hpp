//
// Created by QuzzS on 2025/3/4.
//

#ifndef COMPLETE_SCENE_HPP
#define COMPLETE_SCENE_HPP


#include "App.hpp"
#include "Scene/Scene.hpp"

class CompleteScene : public Scene
{
public:
	CompleteScene() = default;
	~CompleteScene() override = default;

	void Start() override;
	void Input() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
};

#endif //COMPLETE_SCENE_HPP
