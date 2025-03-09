//
// Created by QuzzS on 2025/3/4.
//
#include "Creature/Character.hpp"
#include "Util/Image.hpp"


Character::Character(const std::string& ImagePath, int maxHp, float speed, int aimRange, CollisionBox* radius, Weapon* initialWeapon)
	: m_ImagePath(ImagePath), m_maxHp(maxHp),m_currentHp(maxHp), m_moveSpeed(speed), m_aimRange(aimRange), m_collisionRadius(radius) {
	SetImage(ImagePath);
	ResetPosition();
	if (initialWeapon)
	{
		m_Weapons.push_back(initialWeapon);
		currentWeapon = initialWeapon; // 預設裝備第一把武器
	}
}

void Character::SetImage(const std::string& ImagePath) {
	m_ImagePath = ImagePath;
	m_Drawable = std::make_shared<Util::Image>(m_ImagePath);
}

void Character::takeDamage(int dmg) {
	m_currentHp -= dmg;
	if (m_currentHp < 0) {
		m_currentHp = 0;
	}
	if (m_currentHp == 0) {
		// 角色死亡，執行死亡邏輯
	}
}

bool Character::isDead() const
{
	return m_currentHp <= 0;
}

void Character::AddWeapon(Weapon* weapon) {
	m_Weapons.push_back(weapon);
}

void Character::RemoveWeapon(Weapon* weapon) {
	// auto it = std::find(m_Weapons.begin(), m_Weapons.end(), weapon);
	// if (it != m_Weapons.end()) {
	// 	m_Weapons.erase(it);
	// 	if (currentWeapon == weapon) currentWeapon = m_Weapons.empty() ? nullptr : m_Weapons[0];
	// }
}

Weapon* Character::GetCurrentWeapon() const {
	return currentWeapon;
}