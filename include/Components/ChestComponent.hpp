//
// Created by QuzzS on 2025/4/25.
//

#ifndef CHESTCOMPONENT_HPP
#define CHESTCOMPONENT_HPP

#include "Component.hpp"
namespace Core { class Drawable; }

class ChestComponent final : public Component {
public:
	enum class ChestState
	{
		OPENED = 0,
		CLOSED = 1,
	};

	explicit ChestComponent(ChestType chestType, std::vector<std::string> imagePaths);
	~ChestComponent() override = default;

	void Init() override;
	void Update() override;
	void HandleEvent(const EventInfo& eventInfo) override;
	void HandleCollision(const CollisionEventInfo &info) override;

	/* ---Getter--- */
	[[nodiscard]] ChestState GetCurrentState() const { return m_currentState; }

	/* ---Setter--- */
	void SetState(const ChestState newState) {m_currentState = newState;}
	void AddDropItems(const std::vector<std::shared_ptr<nGameObject>>& items) {m_dropItems = items;}

	void ChestOpened();

private:
	ChestType m_chestType;
	ChestState m_currentState = ChestState::CLOSED;
	std::vector<std::string> m_imagePaths;
	std::vector<std::shared_ptr<Core::Drawable>> m_drawables;
	std::vector<std::shared_ptr<nGameObject>> m_dropItems;
};

#endif //CHESTCOMPONENT_HPP
