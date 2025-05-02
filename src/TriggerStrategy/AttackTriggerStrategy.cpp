//
// Created by QuzzS on 2025/4/29.
//

#include "Creature/Character.hpp"
#include "Structs/TakeDamageEventInfo.hpp"
#include "TriggerStrategy/AttackTriggerStrategy.hpp"

void AttackTriggerStrategy::OnTriggerEnter(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other)
{
	if (!self || !other) return;

	auto target = std::dynamic_pointer_cast<Character>(other);
	if (!target) return;

	// 發送受傷事件
	const TakeDamageEventInfo dmgEvent(m_Damage);
	other->OnEvent(dmgEvent);
}