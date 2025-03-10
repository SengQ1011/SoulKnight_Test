//
// Created by tjx20 on 3/10/2025.
//

#include <utility>

#include "Weapon/Weapon.hpp"

Weapon::Weapon(const std::string&  ImagePath, int damage, int attackRange, int size, int attackSpeed)
	: m_ImagePath(ImagePath), m_damage(damage), m_attackRange(attackRange), m_bulletSize(size), m_attackSpeed(attackSpeed) {}


void Weapon::attack() {
	// 实现攻击逻辑
}