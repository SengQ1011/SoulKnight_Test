//
// Created by tjx20 on 5/9/2025.
//

#include "TriggerStrategy/BlockProjectileStrategy.hpp"
#include "Structs/BlockProjectileEventInfo.hpp"
#include "Attack/Projectile.hpp"

void BlockProjectileStrategy::OnTriggerEnter(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other)
{
	if (!self || !other) return;
	// 这里 other 是和 slash 碰撞到的那个物体
	auto target = std::dynamic_pointer_cast<Projectile>(other);
	if (!target) return;

	// 發送阻擋事件
	const BlockProjectileEventInfo blockEvent;
	other->OnEvent(blockEvent);
}