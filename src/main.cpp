#include "App.hpp"

#include "Core/Context.hpp"
#include <iostream>

int main(int, char**) {
    auto context = Core::Context::GetInstance();
	context->SetWindowIcon(RESOURCE_DIR "/pet00icon.png");
	App app;
	app.SetCurrentScene(std::make_shared<MainMenuScene>());

	// bool isMaximized = false;
	// SDL_Event event;

	// if (event.type == SDL_WINDOWEVENT)
	// {
	// 	switch (event.window.event)
	// 	{
	// 	case SDL_WINDOWEVENT_MAXIMIZED:
	// 		isMaximized = true;
	// 		std::cout << "視窗已最大化" << std::endl;
	// 		break;
	//
	// 	case SDL_WINDOWEVENT_RESTORED:
	// 		if (isMaximized) {
	// 			isMaximized = false;
	// 			std::cout << "視窗已從最大化還原" << std::endl;
	// 		}
	// 		break;
	//
	// 	case SDL_WINDOWEVENT_SIZE_CHANGED:
	// 		std::cout << "視窗大小已變更為: "
	// 				  << event.window.data1 << " x " << event.window.data2
	// 				  << std::endl;
	// 		break;
	// 	}
	// }

    while (!context->GetExit()) {
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

        context->Update();
    }
    return 0;
}
