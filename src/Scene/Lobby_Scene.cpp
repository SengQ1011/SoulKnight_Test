//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/Lobby_Scene.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void LobbyScene::Start()
{
	LOG_DEBUG("Entering Game Scene");
	m_Background->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/Lobby/T.png"));
	m_Background->SetZIndex(0);

	m_Root.AddChild(m_Background);
}

void LobbyScene::Input()
{
	if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB))
	{
		LOG_DEBUG("LobbyScene input \'H\' successfully detected");
		// SDL_RenderGetScale()
	}
}

void LobbyScene::Update()
{
	LOG_DEBUG("Lobby Scene is running...");
	m_Root.Update();
}

void LobbyScene::Exit()
{
	LOG_DEBUG("Lobby Scene exited");
}

Scene::SceneType LobbyScene::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}
