//
// Created by tjx20 on 3/2/2025.
//
#include "Character.hpp"
#include "Util/Image.hpp"

Character::Character(const std::string& ImagePath, int maxHp, double speed, int aimRange, double radius)
	: m_ImagePath(ImagePath), maxHealth(maxHp), currentHealth(maxHp),
	  moveSpeed(speed), aimRange(aimRange), collisionRadius(radius) {
	SetImage(ImagePath);
	ResetPosition();
}

void Character::SetImage(const std::string& ImagePath) {
	m_ImagePath = ImagePath;
	m_Drawable = std::make_shared<Util::Image>(m_ImagePath);
}

