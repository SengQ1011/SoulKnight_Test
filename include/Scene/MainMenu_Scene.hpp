//
// Created by QuzzS on 2025/3/4.
//

#ifndef MAINMENU_SCENE_HPP
#define MAINMENU_SCENE_HPP

#include "Scene/Scene.hpp"

#include "Util/BGM.hpp"
#include "Util/SFX.hpp"


class UIPanel;
class MainMenuScene : public Scene
{
public:
	MainMenuScene() = default;
	~MainMenuScene() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
	std::shared_ptr<Util::GameObject> m_Background = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_Title = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_RedShawl = std::make_shared<Util::GameObject>();

	std::shared_ptr<Util::GameObject> m_Text = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_Version = std::make_shared<Util::GameObject>();
};

#endif //MAINMENU_SCENE_HPP
