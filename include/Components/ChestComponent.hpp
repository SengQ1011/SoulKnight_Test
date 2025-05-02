//
// Created by QuzzS on 2025/4/25.
//

#ifndef CHESTCOMPONENT_HPP
#define CHESTCOMPONENT_HPP

#include "Component.hpp"
namespace Core { class Drawable; }

class ChestComponent final : public Component {
public:
	enum class State
	{
		OPENED = 0,
		CLOSED = 1,
	};

	ChestComponent() = default;
	~ChestComponent() override = default;

	void Init() override;
	void Update() override;
	void HandleEvent(const EventInfo& eventInfo) override;
	void HandleCollision(const CollisionEventInfo &info) override;

	/* ---Getter--- */
	[[nodiscard]] State GetCurrentState() const { return m_currentState; }

	/* ---Setter--- */
	void SetState(const State newState) {m_currentState = newState;}

	void ChestOpened();

private:
	State m_currentState = State::CLOSED;
	std::vector<std::shared_ptr<Core::Drawable>> m_drawables;
};

#endif //CHESTCOMPONENT_HPP
