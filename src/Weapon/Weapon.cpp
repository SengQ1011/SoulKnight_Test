//
// Created by tjx20 on 3/10/2025.
//

#include <utility>

#include "Weapon/Weapon.hpp"

Weapon::Weapon(const std::string& ImagePath, const std::string name, int damage, int energy, float criticalRate, int offset, int dropLevel,  float attackRange, float size, int attackSpeed)
	: m_ImagePath(ImagePath), m_weaponName(name), m_damage(damage), m_energy(energy), m_criticalRate(criticalRate), m_offset(offset), m_dropLevel(dropLevel), m_attackRange(attackRange), m_bulletSize(size), m_attackSpeed(attackSpeed)
{
	SetImage(ImagePath);
}

void Weapon::SetImage(const std::string& ImagePath) {
	m_ImagePath = ImagePath;
	m_Drawable = std::make_shared<Util::Image>(m_ImagePath);
}


void Weapon::attack() {
	// 实现攻击逻辑
}