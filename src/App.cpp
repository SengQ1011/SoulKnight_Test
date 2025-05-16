#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Scene/SceneManager.hpp"

// #include "Tracy.hpp"
// #include <windows.h>
// #include <psapi.h>

inline float GetMemoryUsageMB()
{
	// PROCESS_MEMORY_COUNTERS memInfo;
	// GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
	// return static_cast<float>(memInfo.WorkingSetSize) / (1024.0f * 1024.0f);
}

void App::Start() {
    LOG_TRACE("Start");
    m_CurrentState = State::UPDATE;
	SceneManager::GetInstance().Start();
}

void App::Update() {
	// TracyPlot("FPS", ImGui::GetIO().Framerate); //FPS折綫圖in TracyProfiler
	// TracyPlot("Memory (MB)", GetMemoryUsageMB());
	// TracyPlotConfig("Memory (MB)", tracy::PlotFormatType::Memory, true, 0.0f, 1024.0f);
	// ZoneScopedN("App:Update");
	{
		// ZoneScopedN("ImGui::NewFrame");
		// 测试UI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("FPS Panel");
		ImGui::Text("FPS: %.1f",ImGui::GetIO().Framerate);
		ImGui::End();
	}

    //TODO: do your things here and delete this line <3
	{
		// ZoneScopedN("SceneManager Update");
		SceneManager::GetInstance().ChangeCurrentScene();
		SceneManager::GetInstance().Update();
	}

	{
		// ZoneScopedN("ImGui::Render");
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
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
	SceneManager::GetInstance().End();
}

