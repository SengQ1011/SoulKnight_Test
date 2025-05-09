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
	void Reflect(const glm::vec2& normal) {
		// 以原本方向做反射
		//m_direction = glm::reflect(m_direction, normal);
		// 計算碰撞法線：這裡我們以 slash->子彈 的方向為法線
		// glm::vec2 slashPos = self->m_WorldCoord;
		// glm::vec2 projPos  = other->m_WorldCoord;
		// glm::vec2 normal   = glm::normalize(projPos - slashPos);

		// TODO:把擁有者換成斬擊者

		// 重新計時或重置生命周期
		// ResetLifetime();
	}

};
#endif //PROJECTILECOMPONENT_HPP
