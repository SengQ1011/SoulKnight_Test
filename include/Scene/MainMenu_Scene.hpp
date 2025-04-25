//
// Created by QuzzS on 2025/3/4.
//

#ifndef MAINMENU_SCENE_HPP
#define MAINMENU_SCENE_HPP

#include "Scene/Scene.hpp"

#include "Util/SFX.hpp"
#include "Util/BGM.hpp"


class MainMenuScene : public Scene
{
public:
	MainMenuScene() = default;
	// explicit MainMenuScene(const sceneType = SceneType:) : Scene(sceneType);
	~MainMenuScene() override = default;

	void Start() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
	std::shared_ptr<Util::GameObject> m_Background = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_Title = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_RedShawl = std::make_shared<Util::GameObject>();

	std::shared_ptr<Util::BGM> m_BGM = std::make_shared<Util::BGM>(RESOURCE_DIR"/UI/bgm_openingLow.wav");
	std::shared_ptr<Util::SFX> m_ClickSound = std::make_shared<Util::SFX>(RESOURCE_DIR"/UI/fx_btn1.wav");

	std::shared_ptr<Util::GameObject> m_Text = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_Version = std::make_shared<Util::GameObject>();

	Util::Renderer m_Root;
};

#endif //MAINMENU_SCENE_HPP
