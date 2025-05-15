//
// Created by tjx20 on 5/9/2025.
//

#ifndef PROJECTILECOMPONENT_HPP
#define PROJECTILECOMPONENT_HPP

#include "Components/Component.hpp"

class ProjectileComponent final : public Component {
public:
	explicit ProjectileComponent() = default;
	~ProjectileComponent() override = default;

	std::vector<EventType> SubscribedEventTypes() const override;
	void HandleEvent(const EventInfo &eventInfo) override;
	void HandleCollision(const CollisionEventInfo &info) override;

	// 反彈由策略呼叫
	void HandleReflectEvent();

};
#endif //PROJECTILECOMPONENT_HPP
