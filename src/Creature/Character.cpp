//
// Created by QuzzS on 2025/3/4.
//
#include "Creature/Character.hpp"
#include "Util/Image.hpp"

#include "Creature/Character.hpp"
#include "Util/Image.hpp"

Character::Character(int maxHp, float speed, std::unique_ptr<CollisionBox> collisionBox)
	: m_maxHp(maxHp),m_currentHp(maxHp), m_moveSpeed(speed), m_collisionBox(std::move(collisionBox)) {
	ResetPosition();
	LOG_DEBUG("Character has been initialized");
}


void Character::takeDamage(int dmg) {
	m_currentHp -= dmg;
	if (this->isDead()) {
		m_currentHp = 0;
		//m_state = State::DEAD;
	}
}

bool Character::CheckCollides(const std::shared_ptr<Character> &other) const {
	return false;
}