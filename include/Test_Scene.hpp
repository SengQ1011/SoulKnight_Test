//
// Created by QuzzS on 2025/2/28.
//

#ifndef TEST_SCENE_HPP
#define TEST_SCENE_HPP


#include "App.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "Override/nGameObject.hpp"

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
	std::shared_ptr<nGameObject> m_Tile = std::make_shared<nGameObject>();
	std::shared_ptr<nGameObject> m_Background = std::make_shared<nGameObject>();
	std::shared_ptr<nGameObject> m_Title = std::make_shared<nGameObject>();
	std::shared_ptr<nGameObject> m_RedShawl = std::make_shared<nGameObject>();
	std::shared_ptr<TextObjectUI> m_Text = std::make_shared<TextObjectUI>(RESOURCE_DIR"/Font/zpix.ttf", "點擊開始OwOb" );
	std::shared_ptr<TextObjectUI> m_Version = std::make_shared<TextObjectUI>(RESOURCE_DIR"/Font/zpix.ttf", "版本號 v1.0.0" );

	std::shared_ptr<nGameObject> m_Character = std::make_shared<nGameObject>();
	Camera m_Camera;

	Util::Renderer m_Root;
};

#endif //TEST_SCENE_HPP
