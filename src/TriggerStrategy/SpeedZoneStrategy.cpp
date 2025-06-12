//
// Created by tjx20 on 6/12/2025.
//

#include "TriggerStrategy/SpeedZoneStrategy.hpp"

#include "Creature/Character.hpp"
#include "Structs/SpeedChangeEventInfo.hpp"
#include "Util/Logger.hpp"

void SpeedZoneStrategy::OnTriggerEnter(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other)
{
	if (!self || !other) return;

	// 角色/可被破壞箱子
	auto target = std::dynamic_pointer_cast<Character>(other);
	if (!target) return;

	if (const auto movementComp = target->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
		!movementComp) return;

	const SpeedChangeEventInfo speedChangedEvent(m_speedRatio, m_durationTime);
	other->OnEvent(speedChangedEvent);
}

void SpeedZoneStrategy::OnTriggerStay(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other)
{
	if (!self || !other) return;

	// 角色/可被破壞箱子
	auto target = std::dynamic_pointer_cast<Character>(other);
	if (!target) return;

	if (const auto movementComp = target->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
		!movementComp) return;

	// 在区域内持续触发速度变化
	const SpeedChangeEventInfo speedChangedEvent(m_speedRatio, m_durationTime);
	other->OnEvent(speedChangedEvent);
}