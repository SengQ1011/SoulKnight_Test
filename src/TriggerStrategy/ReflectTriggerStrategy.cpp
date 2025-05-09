//
// Created by tjx20 on 5/9/2025.
//

#include "TriggerStrategy/ReflectTriggerStrategy.hpp"
#include "Attack/Projectile.hpp"
#include "Structs/ReflectProjectileEventInfo.hpp"

void ReflectTriggerStrategy::OnTriggerEnter(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other)
{
	if (!self || !other) return;
	// 確認 other 是子彈
	auto target = std::dynamic_pointer_cast<Projectile>(other);
	if (!target || !target->GetCanReboundBySword()) return;

	// 發送反彈事件
	const ReflectProjectileEventInfo reflectEvent;
	other->OnEvent(reflectEvent);
}