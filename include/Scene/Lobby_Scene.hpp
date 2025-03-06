//
// Created by QuzzS on 2025/3/4.
//

#ifndef LOBBY_SCENE_HPP
#define LOBBY_SCENE_HPP

#include "Scene/Scene.hpp"

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"

class LobbyScene : public Scene
{
public:
	LobbyScene() = default;
	~LobbyScene() override = default;

	void Start() override;
	void Input() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
	std::shared_ptr<Util::GameObject> m_Background = std::make_shared<Util::GameObject>();

	Util::Renderer m_Root;

};

#endif //LOBBY_SCENE_HPP
