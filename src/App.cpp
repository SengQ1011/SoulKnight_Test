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
	SceneManager::GetInstance().Start();
}

void App::Update() {

    //TODO: do your things here and delete this line <3
	SceneManager::GetInstance().ChangeCurrentScene();
	SceneManager::GetInstance().Update();

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
	SceneManager::GetInstance().End();
}