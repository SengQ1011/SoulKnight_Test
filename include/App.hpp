#ifndef APP_HPP
#define APP_HPP

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();

    void Update(); // 游戲循環 （場景切換 + 場景更新）

    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    State m_CurrentState = State::START;
	//TODO: Dungeon預加載指標
};

#endif
