//
// Created by tjx20 on 6/6/2025.
//

#ifndef EFFECTATTACKCOMPONENT_HPP
#define EFFECTATTACKCOMPONENT_HPP

#include "Attack/EffectAttack.hpp"
#include "Components/Component.hpp"

class EffectAttack;
class EffectAttackComponent final : public Component {
public:
	explicit EffectAttackComponent() = default;
	~EffectAttackComponent() override = default;

	void Update() override {}

	std::vector<EventType> SubscribedEventTypes() const override
	{
		return {
			EventType::Collision
		};
	}
	void HandleEvent(const EventInfo &eventInfo) override
	{
		switch (eventInfo.GetEventType())
		{
			case EventType::Collision:
			{
				const auto& collisionEventInfo = dynamic_cast<const CollisionEventInfo&>(eventInfo);
				const auto owner = GetOwner<EffectAttack>();
				if(!owner) LOG_ERROR("don't have");
				owner->SetIsCollisionWithTerrain(true);
				// owner->MarkForRemoval();
				break;
			}
			default:
				break;
		}
	}
};

#endif //EFFECTATTACKCOMPONENT_HPP
