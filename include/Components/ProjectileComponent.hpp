//
// Created by tjx20 on 5/9/2025.
//

#ifndef PROJECTILECOMPONENT_HPP
#define PROJECTILECOMPONENT_HPP

#include "Components/Component.hpp"


class Projectile;
class ProjectileComponent final : public Component {
public:
	explicit ProjectileComponent() = default;
	~ProjectileComponent() override = default;

	void Update() override;

	std::vector<EventType> SubscribedEventTypes() const override;
	void HandleEvent(const EventInfo &eventInfo) override;
	void HandleCollision(const CollisionEventInfo &info) override;
	void HandleReflectEvent();

	void TriggerChainAttack(const std::shared_ptr<Projectile>& projectile);
private:
	bool m_CollisionHandled = false;
};
#endif //PROJECTILECOMPONENT_HPP
