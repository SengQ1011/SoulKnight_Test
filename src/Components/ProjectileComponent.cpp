//
// Created by tjx20 on 5/9/2025.
//
#include "Components/ProjectileComponent.hpp"
#include "Attack/Projectile.hpp"
#include "Creature/Character.hpp"

std::vector<EventType> ProjectileComponent::SubscribedEventTypes() const
{
	return {
		EventType::Collision,
		EventType::BlockedProjectileBySword,
		EventType::ReflectProjectile
	};
}

void ProjectileComponent::HandleEvent(const EventInfo &eventInfo)
{
	// {}可以在case裏形成額外作用域，用來在裏面定義變數
	switch (eventInfo.GetEventType())
	{
		case EventType::Collision:
		{
			const auto& collisionEventInfo = dynamic_cast<const CollisionEventInfo&>(eventInfo);
			HandleCollision(collisionEventInfo);
			break;
		}
		case EventType::BlockedProjectileBySword:
		{
			if (const auto projectile = GetOwner<Projectile>())
				projectile->MarkForRemoval();
			else LOG_WARN("projectile is NULL");
			break;
		}
		case EventType::ReflectProjectile:
		{
			HandleReflectEvent();
			break;
		}
		default:
			break;
		}
}

void ProjectileComponent::HandleCollision(const CollisionEventInfo &info) {
	const auto& other = info.GetObjectB();
	const auto projectile = GetOwner<Projectile>();
	if (!projectile) return;

	const auto numRebound = projectile->GetNumRebound();
	const auto reboundCounter = projectile->GetReboundCounter();

	bool hitTarget = false;

	if (const auto& character = std::dynamic_pointer_cast<Character>(other)) {
		hitTarget = (projectile->GetAttackLayerType() != character->GetType());
	}
	// 可反彈
	if (numRebound > 0 && reboundCounter < numRebound && !hitTarget) {
		const glm::vec2 collisionNormal = info.GetCollisionNormal();
		auto  direction = projectile->GetAttackDirection();
		const float dotProduct = glm::dot(direction, collisionNormal);
		direction = glm::normalize(direction - 2.0f * dotProduct * collisionNormal);
		projectile->SetDirection(direction);

		projectile->m_Transform.rotation = glm::atan(direction.y, direction.x);
		projectile->AddReboundCounter();
	}else projectile->MarkForRemoval();
}

void ProjectileComponent::HandleReflectEvent()
{
	LOG_DEBUG("reflect");
	const auto proj = GetOwner<Projectile>();
	if (!proj) return;
	bool canReflect = proj->GetCanReboundBySword();
	if (!canReflect) {
		LOG_DEBUG("projectile can't reflect");
		proj->MarkForRemoval();
		return;
	}

	auto direction = proj->GetAttackDirection();
	// 以原本方向做反射
	proj->SetDirection(-direction);

	// TODO:把擁有者換成斬擊者
	proj->ReflectChangeAttackCharacterType(CharacterType::PLAYER);
}
