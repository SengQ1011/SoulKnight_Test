#include "App.hpp"

#include <utility>

#include "Scene/Test_Scene_KC.hpp"
#include "Scene/Test_Scene_JX.hpp"
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

	//場景切換 - 主菜單、游戲局内畫面
	Scene::SceneType type = m_CurrentScene->Change();
	if (type != Scene::SceneType::Null)
	{
		m_CurrentScene->Exit(); //退出舊場景
		switch (type)
		{
		case Scene::SceneType::Menu:
			m_CurrentScene = std::make_shared<MainMenuScene>();
			break;
		case Scene::SceneType::Test_KC:
			m_CurrentScene = std::make_shared<TestScene_KC>();
			break;
		case Scene::SceneType::Test_JX:
			m_CurrentScene = std::make_shared<TestScene_JX>();
			break;
		case Scene::SceneType::Lobby:
			m_CurrentScene = std::make_shared<LobbyScene>();
			break;
		case Scene::SceneType::DungeonLoad:
			m_CurrentScene = std::make_shared<DungeonLoadingScene>(); //關卡加載初始化后要先渲染一次畫面，方便關卡預先初始化
			m_PreLoadScene = std::make_shared<DungeonScene>();
			break;
		case Scene::SceneType::Dungeon:
			m_CurrentScene = m_PreLoadScene;
			m_PreLoadScene = nullptr;
			break;
		case Scene::SceneType::Complete:
			m_CurrentScene = std::make_shared<CompleteScene>();
			break;
		case Scene::SceneType::Result:
			m_CurrentScene = std::make_shared<ResultScene>();
			break;
		default:
			break;
		}

		//載入新場景 初始化
		if (type == Scene::SceneType::DungeonLoad)
		{
			m_CurrentScene->Start();
			m_PreLoadScene->Start();
		}
		else if (type != Scene::SceneType::Dungeon)
		{
			m_CurrentScene->Start();
		}
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
	m_CurrentScene = nullptr;
}

void App::SetCurrentScene(std::shared_ptr<Scene> nextScene)
{
	m_CurrentScene = std::move(nextScene);
}