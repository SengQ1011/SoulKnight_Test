//
// Created by QuzzS on 2025/3/4.
//
#include "Creature/Character.hpp"
#include "Util/Image.hpp"


Character::Character(const std::string& ImagePath, int maxHp, float speed, int aimRange, CollisionBox* radius, std::unique_ptr<Weapon> initialWeapon)
	: m_ImagePath(ImagePath), m_maxHp(maxHp),m_currentHp(maxHp), m_moveSpeed(speed), m_aimRange(aimRange), m_collisionRadius(radius), currentWeapon(std::move(initialWeapon)) {
	SetImage(ImagePath);
	ResetPosition();
}

void Character::SetImage(const std::string& ImagePath) {
	m_ImagePath = ImagePath;
	m_Drawable = std::make_shared<Util::Image>(m_ImagePath);
}

void Character::attack() {
	if (currentWeapon) {
		currentWeapon->attack();
	}
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

void Character::RemoveWeapon(Weapon* weapon) {
	auto it = std::find_if(m_Weapons.begin(), m_Weapons.end(),
		[weapon](const std::unique_ptr<Weapon>& w) { return w.get() == weapon; });

	if (it != m_Weapons.end()) {
		m_Weapons.erase(it);  // 根據指標刪除武器
	}
}

void Character::AddWeapon(std::unique_ptr<Weapon> newWeapon) {
	if (m_Weapons.empty()) {
		// 注意： unique_ptr不能被複製==》需要 std::move()
		m_Weapons.push_back(std::move(newWeapon));
	}
}


bool Character::CheckCollides(const std::shared_ptr<Character> &other) const {

	return false;
}
