//
// Created by tjx20 on 3/26/2025.
//

#include "Weapon/Bullet.hpp"

#include "Components/CollisionComponent.hpp"
#include "Creature/Character.hpp"

 Bullet::Bullet(const CharacterType type, const std::string& ImagePath, const Util::Transform &bulletTransform, glm::vec2 direction, float size, float speed, int damage, int numRebound)
	: m_type(type), m_imagePath(ImagePath), m_direction(direction), m_size(size), m_speed(speed), m_damage(damage), m_numRebound(numRebound)
 {
 	this->m_Transform = bulletTransform;
 	//LOG_DEBUG("Bullet.cpp::Bullet is created==>{}",this->m_WorldCoord);
 }
// 靜態成員變數
std::unordered_map<std::string, std::shared_ptr<Util::Image>> Bullet::sharedImages;


void Bullet::Init() {
	// 明確設定世界坐標（從傳入的 Transform 取得）
	this->m_WorldCoord = m_Transform.translation;
	m_startPosition = this->m_WorldCoord;
	// 其他初始化（縮放、圖片等）
	this->SetInitialScale(m_Transform.scale);
	this->SetZIndexType(ZIndexType::BULLET);
	SetImage(m_imagePath);

	auto CollisionComp = this->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (!CollisionComp) { CollisionComp = this->AddComponent<CollisionComponent>(ComponentType::COLLISION); }
	CollisionComp->ResetCollisionMask();
	CollisionComp->SetTrigger(false);
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
 	CollisionComp->SetSize(glm::vec2(10.0f));
	//CollisionComp->SetSize(glm::vec2(m_size));

 	// auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
 	// currentScene->GetRoot().lock()->AddChild(CollisionComp->GetVisibleBox());
 	// currentScene->GetCamera().lock()->AddChild(CollisionComp->GetVisibleBox());
 }

void Bullet::UpdateLocation(const float deltaTime) {
 	// 讓子彈按方向移動
 	this->m_WorldCoord += m_direction * m_speed * deltaTime;
	if(glm::distance(this->m_WorldCoord, m_startPosition) >= 1000.0f)
	{
		this->MarkForRemoval();
	}
 }

// 子彈資源共用（圖片資源共享）
void Bullet::SetImage(const std::string& imagePath) {
	m_imagePath = imagePath;
	if (sharedImages.find(imagePath) == sharedImages.end()) {
		sharedImages[imagePath] = std::make_shared<Util::Image>(imagePath);
	}
	m_Drawable = sharedImages[imagePath];
}

void Bullet::onCollision(const std::shared_ptr<nGameObject> &other, CollisionInfo &info) {
 	bool hitTarget = false;
	if (std::shared_ptr<Character> character = std::dynamic_pointer_cast<Character>(other)){
		if(m_type != character->GetType()) hitTarget = true;
	}
 	// 檢查子彈是否還有反彈次數
 	if (m_numRebound > 0 && m_reboundCounter < m_numRebound && !hitTarget){
 		// 獲取碰撞法線
 		glm::vec2 collisionNormal = info.GetCollisionNormal();

 		// 反彈方向 = 方向 - 2 * (方向·法線) * 法線
		const float dotProduct = glm::dot(m_direction, collisionNormal);  // 方向和法線的點積
 		m_direction = m_direction - 2.0f * dotProduct * collisionNormal;  // 更新方向

 		m_direction = glm::normalize(m_direction);

 		// 調整旋轉角度
		const float angle = glm::atan(m_direction.y, m_direction.x);
 		this->m_Transform.rotation = angle;

 		m_reboundCounter++;
 	}
 	else {
 		// 如果沒有反彈次數了或是擊中目標，標記為移除
 		MarkForRemoval();
 	}
 }

void Bullet::ResetAll(const CharacterType type, const std::string& ImagePath, const Util::Transform &bulletTransform,
					   const glm::vec2 direction, const float size, const float speed, const int damage, const int numRebound){
	m_type = type;
	m_imagePath = ImagePath;
	this->m_Transform = bulletTransform;
	m_direction = direction;
	m_size = size;
	m_speed = speed;
	m_damage = damage;
	m_numRebound = numRebound;
	m_hasHit = false;
	m_reboundCounter = 0;
}
