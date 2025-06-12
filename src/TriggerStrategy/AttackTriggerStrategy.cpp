//
// Created by QuzzS on 2025/4/29.
//

#include "TriggerStrategy/AttackTriggerStrategy.hpp"
#include "Creature/Character.hpp"
#include "Structs/TakeDamageEventInfo.hpp"

#include "RoomObject/DestructibleObject.hpp"

void AttackTriggerStrategy::OnTriggerEnter(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other)
{
	if (!self || !other)
		return;
	int ID = self->GetID();

	// 檢查是否為角色
	auto character = std::dynamic_pointer_cast<Character>(other);
	if (character)
	{
		if (const auto healthComp = character->GetComponent<HealthComponent>(ComponentType::HEALTH); !healthComp)
			return;

		const TakeDamageEventInfo dmgEvent(ID, m_Damage, m_elementalDamage);
		other->OnEvent(dmgEvent);
		return;
	}

	// 檢查是否為可破壞箱子
	auto destructibleObject = std::dynamic_pointer_cast<DestructibleObject>(other);
	if (destructibleObject)
	{
		if (const auto healthComp = destructibleObject->GetComponent<HealthComponent>(ComponentType::HEALTH);
			!healthComp)
			return;

		const TakeDamageEventInfo dmgEvent(ID, m_Damage, m_elementalDamage);
		other->OnEvent(dmgEvent);
		return;
	}

	// 既不是角色也不是箱子，直接返回
}
