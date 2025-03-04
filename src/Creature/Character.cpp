//
// Created by QuzzS on 2025/3/4.
//
#include "Creature/Character.hpp"
#include "Util/Image.hpp"

Character::Character(const std::string& ImagePath, int maxHp, float speed, int aimRange, double radius)
	: m_ImagePath(ImagePath), maxHealth(maxHp), currentHealth(maxHp),
	  moveSpeed(speed), aimRange(aimRange), collisionRadius(radius) {
	SetImage(ImagePath);
	ResetPosition();
}

void Character::SetImage(const std::string& ImagePath) {
	m_ImagePath = ImagePath;
	m_Drawable = std::make_shared<Util::Image>(m_ImagePath);
}
