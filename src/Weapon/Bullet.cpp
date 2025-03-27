//
// Created by tjx20 on 3/26/2025.
//

#include "Weapon/Bullet.hpp"
#include "Util/Time.hpp"

 Bullet::Bullet(const std::string& ImagePath,Util::Transform bulletTransform, glm::vec2 direction, float speed, int damage)
	: m_imagePath(ImagePath), m_direction(direction), m_speed(speed), m_damage(damage)
 {
 	// 明確設定世界坐標（從傳入的 Transform 取得）
 	this->m_Transform = bulletTransform;
 	this->m_WorldCoord = bulletTransform.translation;
 	// 其他初始化（縮放、圖片等）
 	this->m_Transform.scale = glm::vec2(0.7f, 0.7f);
 	SetImage(ImagePath);
 	this->SetZIndex(20);
 	LOG_DEBUG("Bullet is created");
 }


bool Bullet::isOutOfBounds() const {
 	static const float BOUNDARY = 200.0f;
 	return abs(m_Transform.translation.x) > BOUNDARY ||
			abs(m_Transform.translation.y) > BOUNDARY;
 }

void Bullet::Update(float deltaTime) {
 	// 讓子彈按方向移動
 	this->m_WorldCoord += m_direction * m_speed * deltaTime;
 	this->m_Transform.translation = this->m_WorldCoord;
}

void Bullet::SetImage(const std::string& ImagePath) {
 	m_imagePath = ImagePath;
 	m_Drawable = std::make_shared<Util::Image>(m_imagePath);
 }