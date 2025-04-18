//
// Created by tjx20 on 4/11/2025.
//

#include "Weapon/Slash.hpp"

#include "Scene/SceneManager.hpp"

Slash::Slash(const CharacterType type, const std::shared_ptr<Animation> &anim, const float range): m_type(type), m_animation(anim), m_attackRange(range)
{
	LOG_DEBUG("Slash::Slash");
}

void Slash::Init() {
	// 只初始化碰撞組件，不加入渲染器
	this->SetDrawable(m_animation->GetDrawable());
	m_animation->PlayAnimation(true);

	const auto collisionComp = this->AddComponent<CollisionComponent>(ComponentType::COLLISION);
	if(m_type == CharacterType::PLAYER) {
		collisionComp->SetCollisionLayer(CollisionLayers_Player_Bullet);
		collisionComp->SetCollisionMask(CollisionLayers_Enemy);
	}
	else if (m_type == CharacterType::ENEMY) {
		collisionComp->SetCollisionLayer(CollisionLayers_Enemy_Bullet);
		collisionComp->SetCollisionMask(CollisionLayers_Player);
	}

	collisionComp->SetSize(glm::vec2(8.0f));

	// auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	// currentScene->GetRoot().lock()->AddChild(collisionComp->GetVisibleBox());
	// currentScene->GetCamera().lock()->AddChild(collisionComp->GetVisibleBox());
}


void Slash::Update() {
	LOG_DEBUG("Slash::Update");
 	if (m_animation->IfAnimationEnds()) {
 		SetActive(false);
		const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
 		// scene->GetRoot().lock()->RemoveChild(shared_from_this());
 		// scene->GetCamera().lock()->RemoveChild(shared_from_this());
 		// scene->GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION)->UnregisterNGameObject(shared_from_this());
 	}

 }

