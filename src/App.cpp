#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Scene/SceneManager.hpp"

void App::Start() {
    LOG_TRACE("Start");
    m_CurrentState = State::UPDATE;
	SceneManager::GetInstance().Start();
}

void App::Update() {
	// 测试UI
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("FPS Panel");
	ImGui::Text("FPS: %.1f",ImGui::GetIO().Framerate);
	ImGui::End();

    //TODO: do your things here and delete this line <3

	SceneManager::GetInstance().ChangeCurrentScene();
	SceneManager::GetInstance().Update();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

