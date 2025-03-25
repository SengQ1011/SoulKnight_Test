//
// Created by QuzzS on 2025/3/4.
//
#include "Creature/Character.hpp"
#include "Util/Image.hpp"

#include "Creature/Character.hpp"
#include "Util/Image.hpp"

Character::Character(int maxHp, float speed, int aimRange, std::unique_ptr<CollisionBox> collisionBox, std::shared_ptr<Weapon> initialWeapon)
	: m_maxHp(maxHp),m_currentHp(maxHp), m_moveSpeed(speed), m_aimRange(aimRange), m_collisionBox(std::move(collisionBox)), m_currentWeapon(initialWeapon) {
	ResetPosition();
	AddWeapon(initialWeapon);

	if (m_currentWeapon) {
		this->AddChild(m_currentWeapon);  // 確保武器加入角色
		LOG_DEBUG("weapon follower");
	}
	LOG_DEBUG("Character has been initialized");
}

void Character::Update(float deltaTime) {
	UpdateComponents(deltaTime);
}

void Character::UpdateComponents(float deltaTime) {
	for (auto& [type, component] : m_Components) {
		component->Update();  // 更新每個組件
	}
}



void Character::attack() {
	//m_state = State::ATTACK;
	if (m_currentWeapon) {
		m_currentWeapon->attack();
	}
}

void Character::takeDamage(int dmg) {
	m_currentHp -= dmg;
	if (this->isDead()) {
		m_currentHp = 0;
		//m_state = State::DEAD;
	}
}

void Character::RemoveWeapon(Weapon* weapon) {
	auto it = std::find_if(m_Weapons.begin(), m_Weapons.end(),
		[weapon](std::shared_ptr<Weapon> w) { return w.get() == weapon; });

	if (it != m_Weapons.end()) {
		m_Weapons.erase(it);
	}
}

void Character::AddWeapon(std::shared_ptr<Weapon> newWeapon) {
	if (m_Weapons.empty()) {
		m_Weapons.push_back(newWeapon);
	}
}

bool Character::CheckCollides(const std::shared_ptr<Character> &other) const {
	return false;
}