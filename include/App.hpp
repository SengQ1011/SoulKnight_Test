#ifndef APP_HPP
#define APP_HPP

#include "Scene/Scene.hpp"
#include "Scene/Lobby_Scene.hpp"
#include "Scene/Game_Scene.hpp"
#include "Scene/MainMenu_Scene.hpp"

#include "pch.hpp" // IWYU pragma: export

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

	//場景初始化用功能
	void SetCurrentScene(std::shared_ptr<Scene> nextScene);

    void Start();

    void Update(); // 游戲循環 （場景切換 + 場景更新）

    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    void ValidTask();

private:
    State m_CurrentState = State::START;
	std::shared_ptr<Scene> m_CurrentScene;
};

#endif
