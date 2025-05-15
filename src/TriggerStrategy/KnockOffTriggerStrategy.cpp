//
// Created by tjx20 on 5/15/2025.
//

#include "TriggerStrategy/KnockOffTriggerStrategy.hpp"

#include "Creature/Character.hpp"
#include "Structs/KnockOffEventInfo.hpp"

void KnockOffTriggerStrategy::OnTriggerEnter(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other)
{
	if (!self || !other) return;
	// 这里 other 是和 shockwace 碰撞到的那个物体
	auto target = std::dynamic_pointer_cast<Character>(other);
	if (!target) return;


	// 從 shockwave 發射位置指向敵人方向
	glm::vec2 direction = glm::normalize(other->m_WorldCoord - self->m_WorldCoord);

	// 推力大小可調
	const glm::vec2 impulseVelocity = direction * m_impulseForce;

	// 發送阻擋事件
	const KnockOffEventInfo knockOffEvent(impulseVelocity);
	other->OnEvent(knockOffEvent);
}