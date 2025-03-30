//
// Created by tjx20 on 3/26/2025.
//

#include "Weapon/Bullet.hpp"

#include "Components/CollisionComponent.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Time.hpp"

 Bullet::Bullet(const std::string& ImagePath,Util::Transform bulletTransform, glm::vec2 direction, float speed, int damage)
	: m_imagePath(ImagePath), m_direction(direction), m_speed(speed), m_damage(damage)
 {
 	this->m_Transform = bulletTransform;
 	// 明確設定世界坐標（從傳入的 Transform 取得）
 	this->m_WorldCoord = bulletTransform.translation;
 	// 其他初始化（縮放、圖片等）
 	this->SetInitialScale(bulletTransform.scale);
 	this->SetZIndex(20);
 	SetImage(ImagePath);
 	LOG_DEBUG("Bullet.cpp::Bullet is created==>{}",this->m_WorldCoord);
 }

void Bullet::PostInitialize() {
 	auto CollisionComp = this->AddComponent<CollisionComponent>(ComponentType::COLLISION);
 	CollisionComp->SetCollisionLayer(CollisionLayers_Player_Bullet);
 	CollisionComp->SetCollisionMask(CollisionLayers_Enemy);
 	CollisionComp->SetSize(glm::vec2(10.0f));
 }

void Bullet::UpdateLocation(float deltaTime) {
 	// 讓子彈按方向移動
 	this->m_WorldCoord += m_direction * m_speed * deltaTime;
 }

void Bullet::SetImage(const std::string& ImagePath) {
 	m_imagePath = ImagePath;
 	m_Drawable = std::make_shared<Util::Image>(m_imagePath);
 }

