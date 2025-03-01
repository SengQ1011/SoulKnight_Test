#include "App.hpp"

#include <utility>

#include "Test_Scene.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void App::Start() {
    LOG_TRACE("Start");
    m_CurrentState = State::UPDATE;
	m_CurrentScene->Start();
}

void App::Update() {

    //TODO: do your things here and delete this line <3

	//場景切換 - 主菜單、游戲局内畫面 TODO 這裏應該搬去Scene的Change裏面實作， 然後每次呼叫判斷？有才換 沒就skip
	Scene::SceneType type = m_CurrentScene->Change();
	if (type != Scene::SceneType::Null)
	{
		m_CurrentScene->Exit(); //退出舊場景
		switch (type)
		{
		case Scene::SceneType::Menu:
			m_CurrentScene = std::make_shared<MainMenuScene>();
			break;
		case Scene::SceneType::Test:
			m_CurrentScene = std::make_shared<TestScene>();
			break;
		case Scene::SceneType::Lobby:
			m_CurrentScene = std::make_shared<LobbyScene>();
			break;
		case Scene::SceneType::Game:
			m_CurrentScene = std::make_shared<GameScene>();
			break;
		default:
			break;
		}
		m_CurrentScene->Start(); //載入新場景
	}

	m_CurrentScene->Input();
	m_CurrentScene->Update(); //場景更新

    /*
     * Do not touch the code below as they serve the purpose for
     * closing the window.
     */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
	m_CurrentScene->Exit();
}

void App::SetCurrentScene(std::shared_ptr<Scene> nextScene)
{
	m_CurrentScene = std::move(nextScene);
}





