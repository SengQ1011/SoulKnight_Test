//
// Created by QuzzS on 2025/2/23.
//

#ifndef MENU_SCENE_HPP
#define MENU_SCENE_HPP

#include "App.hpp"
#include "Button_UI.hpp"
#include "Scene.hpp"

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "TextObject_UI.hpp"

class MainMenuScene : public Scene
{
public:
	MainMenuScene() = default;
	~MainMenuScene() override = default;

	void Start() override;
	void Input() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

private:
	std::shared_ptr<Util::GameObject> m_Background = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_Title = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_RedShawl = std::make_shared<Util::GameObject>();

	std::shared_ptr<TextObjectUI> m_Text = std::make_shared<TextObjectUI>(RESOURCE_DIR"/Font/zpix.ttf", "點擊開始OwOb" );
	std::shared_ptr<TextObjectUI> m_Version = std::make_shared<TextObjectUI>(RESOURCE_DIR"/Font/zpix.ttf", "版本號 v1.0.0" );

	Util::Renderer m_Root;
};

#endif //MENU_SCENE_HPP
