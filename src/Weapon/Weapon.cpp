//
// Created by tjx20 on 3/10/2025.
//

#include <utility>

#include "Weapon/Weapon.hpp"

Weapon::Weapon(const std::string& ImagePath, const std::string& bulletImagePath,const std::string& name, int damage, int energy, float criticalRate, int offset, float attackSpeed)
	: m_ImagePath(ImagePath), m_bulletImagePath(bulletImagePath),m_weaponName(name), m_damage(damage), m_energy(energy), m_criticalRate(criticalRate), m_offset(offset), m_attackSpeed(attackSpeed)
{
	SetImage(ImagePath);
	this->SetZIndex(15);
}

bool Weapon::CanAttack(float deltaTime) {
	lastAttackTime += deltaTime / 1000.0f;		// 轉換單位
	if (lastAttackTime >= m_attackSpeed) {
		lastAttackTime = 0.0f;
		return true;
	}
	return false;
}

void Weapon::SetImage(const std::string& ImagePath) {
	m_ImagePath = ImagePath;
	m_Drawable = std::make_shared<Util::Image>(m_ImagePath);
}