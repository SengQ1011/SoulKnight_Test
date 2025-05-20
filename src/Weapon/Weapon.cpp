//
// Created by tjx20 on 3/10/2025.
//

#include "Weapon/Weapon.hpp"
#include "Util/Image.hpp"

Weapon::Weapon(const BaseWeaponInfo& weaponInfo)
	: m_ImagePath(weaponInfo.imagePath),
		m_weaponName(weaponInfo.name),
		m_AttackType(weaponInfo.attackType),
		m_weaponType(weaponInfo.weaponType),
		m_damage(weaponInfo.damage),
		m_energy(weaponInfo.energy),
		m_criticalRate(weaponInfo.criticalRate),
		m_offset(weaponInfo.offset),
		m_attackInterval(weaponInfo.attackInterval)
{
	SetImage(weaponInfo.imagePath);
	this->SetZIndexType(ZIndexType::OBJECTLOW);
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