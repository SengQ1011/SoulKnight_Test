//
// Created by QuzzS on 2025/4/24.
//

#ifndef DOORCOMPONENT_HPP
#define DOORCOMPONENT_HPP

#include "Component.hpp"
#include "ObserveManager/EventManager.hpp"

namespace Core
{
	class Drawable;
}

/**
 * @brief 門的專屬物件，需要CollisionComponent
 */
class DoorComponent final : public Component
{
public:
	enum class State
	{
		OPENED = 0,
		CLOSED = 1,
	};

	DoorComponent();
	~DoorComponent() override;

	void Init() override;
	void Update() override;

	/* ---Getter--- */
	[[nodiscard]] State GetCurrentState() const { return m_currentState; }

	/* ---Setter--- */
	void SetState(const State newState) { m_currentState = newState; }

	void DoorOpened();
	void DoorClosed();

private:
	State m_currentState = State::OPENED;
	std::vector<std::shared_ptr<Core::Drawable>> m_drawables;

	// 事件監聽器ID
	EventManager::ListenerID m_DoorOpenListenerID = 0;
	EventManager::ListenerID m_DoorCloseListenerID = 0;
};

#endif // DOORCOMPONENT_HPP
