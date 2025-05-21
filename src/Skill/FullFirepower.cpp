//
// Created by tjx20 on 3/29/2025.
//

#include "Weapon/Weapon.hpp"
#include "Components/AttackComponent.hpp"
#include "Skill/FullFirepower.hpp"
#include "Components/FollowerComponent.hpp"

FullFirepower::FullFirepower(const std::weak_ptr<Character> &m_owner,const std::string& name,
							 const std::string &iconPath, const float skillDuration, const float cooldownTime)
: Skill(m_owner, name, iconPath, skillDuration, cooldownTime) {}

void FullFirepower::EndSkill(){
	auto attackComp = m_owner.lock()->GetComponent<AttackComponent>(ComponentType::ATTACK);
	attackComp->SetDualWield(false);
}

void FullFirepower::Execute() {
	if (auto attackComp = m_owner.lock()->GetComponent<AttackComponent>(ComponentType::ATTACK)) {
		if(auto currentWeapon = attackComp->GetCurrentWeapon())
		{
			auto attackType = currentWeapon->GetAttackType();
			auto weaponType = currentWeapon->GetWeaponType();
			auto cloneWeapon = currentWeapon->Clone();
			auto cloneFollowerComp = cloneWeapon->AddComponent<FollowerComponent>(ComponentType::FOLLOWER);
			auto currentFollowerComp = currentWeapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER);
			cloneFollowerComp->SetFollower(m_owner.lock());
			cloneFollowerComp->SetTargetMouse(currentFollowerComp->GetUseMouse());
			cloneFollowerComp->SetHandOffset(glm::vec2(30/5.0f,-25/4.0f));
			cloneFollowerComp->SetHoldingPosition(glm::vec2(30/2.0f,2.0f));
			if(Weapon::weaponHasOffset(attackType, weaponType))
				cloneFollowerComp->SetIsSword(true);
			attackComp->SetSecondWeapon(cloneWeapon);
			attackComp->SetDualWield(true);
			cloneWeapon->SetZIndexType(ZIndexType::CUSTOM);
			cloneWeapon->SetZIndex(currentWeapon->GetZIndex() - 0.5f);


			//設置跟隨的ZIndex
			cloneFollowerComp->SetZIndexOffset(currentFollowerComp->GetZIndexOffset());
			// 设置副武器延迟响应
			cloneWeapon->SetAttackDelay(0.15f); // 0.15秒延迟
			isActive = true;
			m_remainingDuration = m_skillDuration;
			m_remainingCD = m_CDTime;
		}
	}
}