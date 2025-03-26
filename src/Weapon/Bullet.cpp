//
// Created by tjx20 on 3/26/2025.
//

#include "Weapon/Bullet.hpp"

 Bullet::Bullet(const std::string& ImagePath,Util::Transform bulletTransform, glm::vec2 direction, float speed, int damage)
	: m_imagePath(ImagePath), m_direction(direction), m_speed(speed), m_damage(damage)
 {
 	this->m_Transform = bulletTransform;
 	this->m_Transform.scale = glm::vec2(0.5f, 0.5f);
 	SetImage(m_imagePath);
 	this->SetZIndex(20);
 	LOG_DEBUG("Bullet is created");
 }

bool Bullet::isOutOfBounds() const {
 	return this->m_Transform.translation.x < -200.0f ||
 		this->m_Transform.translation.x > 200.0f ||
 			this->m_Transform.translation.y < -200.0f ||
 				this->m_Transform.translation.y > 200.0f;
 }
void Bullet::Update() {
 	// 讓子彈按方向移動
 	this->m_Transform.translation += m_direction * m_speed;
 }

void Bullet::SetImage(const std::string& ImagePath) {
 	m_imagePath = ImagePath;
 	m_Drawable = std::make_shared<Util::Image>(m_imagePath);
 }