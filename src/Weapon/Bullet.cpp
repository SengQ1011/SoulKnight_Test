//
// Created by tjx20 on 3/26/2025.
//

#include "Weapon/Bullet.hpp"

#include "Components/CollisionComponent.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Time.hpp"

 Bullet::Bullet(const CharacterType type, const std::string& ImagePath,Util::Transform bulletTransform, glm::vec2 direction, float size, float speed, int damage)
	: m_type(type), m_imagePath(ImagePath), m_direction(direction), m_size(size), m_speed(speed), m_damage(damage)
 {
 	this->m_Transform = bulletTransform;
 	// 明確設定世界坐標（從傳入的 Transform 取得）
 	this->m_WorldCoord = bulletTransform.translation;
 	// 其他初始化（縮放、圖片等）
 	this->SetInitialScale(bulletTransform.scale);
 	this->SetZIndex(10);
 	SetImage(ImagePath);
 	LOG_DEBUG("Bullet.cpp::Bullet is created==>{}",this->m_WorldCoord);
 }

void Bullet::PostInitialize() {
 	auto CollisionComp = this->AddComponent<CollisionComponent>(ComponentType::COLLISION);
	if(m_type == CharacterType::PLAYER) {
		CollisionComp->SetCollisionLayer(CollisionLayers_Player_Bullet);
		CollisionComp->SetCollisionMask(CollisionLayers_Enemy);
	}
 	else if (m_type == CharacterType::ENEMY) {
 		CollisionComp->SetCollisionLayer(CollisionLayers_Enemy_Bullet);
 		CollisionComp->SetCollisionMask(CollisionLayers_Player);
 	}
 	CollisionComp->SetCollisionMask(CollisionLayers_Terrain);

 	// TODO:測試子彈大小
 	CollisionComp->SetSize(glm::vec2(16));

 	auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
 	currentScene->GetRoot().lock()->AddChild(CollisionComp->GetBlackBox());
 	currentScene->GetCamera().lock()->AddRelativePivotChild(CollisionComp->GetBlackBox());
 }

void Bullet::UpdateLocation(float deltaTime) {
 	// 讓子彈按方向移動
 	auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
 	this->m_WorldCoord += m_direction * m_speed * deltaTime;
 }

void Bullet::SetImage(const std::string& ImagePath) {
 	m_imagePath = ImagePath;
 	m_Drawable = std::make_shared<Util::Image>(m_imagePath);
 }

