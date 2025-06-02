//
// Created by QuzzS on 2025/4/29.
//

#include "Creature/Character.hpp"
#include "Structs/TakeDamageEventInfo.hpp"
#include "TriggerStrategy/AttackTriggerStrategy.hpp"

void AttackTriggerStrategy::OnTriggerEnter(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other)
{
	if (!self || !other) return;
	int ID = self->GetID();

	// 角色/可被破壞箱子
	auto target = std::dynamic_pointer_cast<Character>(other);
	if (!target)
	{
		// TODO:箱子class
		// if(auto target = std::dynamic_pointer_cast<Character>(other);
		// 	target != nullptr)
			return;
	}
	if (const auto healthComp = target->GetComponent<HealthComponent>(ComponentType::HEALTH);
		!healthComp) return;

	const TakeDamageEventInfo dmgEvent(ID, m_Damage, m_elementalDamage);
	other->OnEvent(dmgEvent);
}