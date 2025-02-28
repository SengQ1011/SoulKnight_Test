//
// Created by QuzzS on 2025/2/28.
//

#ifndef TEST_SCENE_HPP
#define TEST_SCENE_HPP


#include "App.hpp"
#include "Scene.hpp"
#include "Camera.hpp"

class TestScene : public Scene
{
public:
	TestScene() = default;
	~TestScene() override = default;

	void Start() override;
	void Input() override;
	void Update() override;
	void Exit() override;
	SceneType Change() override;

protected:
	std::shared_ptr<Util::GameObject> m_Background = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_Title = std::make_shared<Util::GameObject>();
	std::shared_ptr<Util::GameObject> m_RedShawl = std::make_shared<Util::GameObject>();
	std::shared_ptr<TextObjectUI> m_Text = std::make_shared<TextObjectUI>(RESOURCE_DIR"/Font/zpix.ttf", "點擊開始OwOb" );
	std::shared_ptr<TextObjectUI> m_Version = std::make_shared<TextObjectUI>(RESOURCE_DIR"/Font/zpix.ttf", "版本號 v1.0.0" );

	std::shared_ptr<Util::GameObject> m_Character = std::make_shared<Util::GameObject>();
	Camera m_Camera;

	Util::Renderer m_Root;
};

#endif //TEST_SCENE_HPP
