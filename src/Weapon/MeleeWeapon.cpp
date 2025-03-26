//
// Created by tjx20 on 3/26/2025.
//
#include "Weapon/MeleeWeapon.hpp"

MeleeWeapon::MeleeWeapon(const std::string &ImagePath, const std::string& bulletImagePath, const std::string &name, int damage, int energy, float criticalRate,
						int offset, float attackSpeed, float attackRange)
						: Weapon(ImagePath, bulletImagePath,name, damage, energy, criticalRate, offset, attackSpeed),
							m_attackRange(attackRange){}

void MeleeWeapon::attack(int damage) {
	LOG_DEBUG("MeleeWeapon attack");
}