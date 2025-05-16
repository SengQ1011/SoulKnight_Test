#include "App.hpp"
#include "Scene/MainMenu_Scene.hpp"

#include <iostream>
#include "Core/Context.hpp"

// #include "Tracy.hpp"
// #include "MemoryTracker.hpp"

int main(int, char**) {
    auto context = Core::Context::GetInstance();
	context->SetWindowIcon(RESOURCE_DIR "/pet00icon.png");
	App app;

	while (!context->GetExit()) {
		// ZoneScopedN("mainUpdate"); // 一幀完整
		switch (app.GetCurrentState()) {
		case App::State::START:
			app.Start();
			break;

		case App::State::UPDATE:
			app.Update();
			break;

		case App::State::END:
			app.End();
			context->SetExit(true);
			break;
		}
		{
			// ZoneScopedN("contextUpdate"); // 視窗更新
			context->Update();
		}
		// FrameMark; //告訴TracyProfiler哪一幀結束，用來收集數據
	}

    return 0;
}
