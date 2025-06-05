//
// Created by tjx20 on 3/10/2025.
//

#include "Weapon/Weapon.hpp"
#include "Util/Image.hpp"

Weapon::Weapon(const BaseWeaponInfo& weaponInfo)
	:	m_ID(weaponInfo.id),
		m_ImagePath(weaponInfo.imagePath),
		m_weaponName(weaponInfo.name),
		m_AttackType(weaponInfo.attackType),
		m_weaponType(weaponInfo.weaponType),
		m_damage(weaponInfo.damage),
		m_energy(weaponInfo.energy),
		m_criticalRate(weaponInfo.criticalRate),
		m_offset(weaponInfo.offset),
		m_attackInterval(weaponInfo.attackInterval),
		m_attackInitPositionOffset(weaponInfo.attackInitPositionOffset),
		m_dropLevel(weaponInfo.dropLevel),
		m_basicPrice(weaponInfo.basicPrice)

{
	SetImage(weaponInfo.imagePath);
	this->SetZIndexType(ZIndexType::OBJECTHIGH);
}

void Weapon::UpdateCooldown(float deltaTime) {
	if (lastAttackTime > 0) {
		lastAttackTime -= deltaTime;
	}
}

bool Weapon::CanAttack() {
	return lastAttackTime <= 0;
}

void Weapon::SetImage(const std::string& ImagePath) {
	m_ImagePath = ImagePath;
	m_Drawable = ImagePoolManager::GetInstance().GetImage(m_ImagePath);
}