//
// Created by tjx20 on 4/19/2025.
//

#include "Attack/EffectAttack.hpp"
#include "Scene/SceneManager.hpp"
#include "Components/CollisionComponent.hpp"

EffectAttack::EffectAttack(const CharacterType type, const Util::Transform &attackTransform, glm::vec2 direction,float size, int damage, bool canReflect, EffectAttackType effectType)
				:Attack(type, attackTransform, direction, size, damage), m_reflectBullet(canReflect), m_effectType(effectType) {}

void EffectAttack::Init() {
	// 明確設定世界坐標（從傳入的 Transform 取得）
	this->m_WorldCoord = m_Transform.translation;
	// Animation
	const auto& imagePaths = EffectAssets::EFFECT_IMAGE_PATHS.at(m_effectType);
 	m_animation = std::make_shared<Animation>(imagePaths, false);
 	this->SetDrawable(m_animation->GetDrawable());
 	m_animation->PlayAnimation(true);

 	// 只初始化碰撞組件，不加入渲染器
 	const auto collisionComp = this->AddComponent<CollisionComponent>(ComponentType::COLLISION);
 	if(m_type == CharacterType::PLAYER) {
 		collisionComp->SetCollisionLayer(CollisionLayers_Player_Bullet);
 		collisionComp->SetCollisionMask(CollisionLayers_Enemy);
 	}
 	else if (m_type == CharacterType::ENEMY) {
 		collisionComp->SetCollisionLayer(CollisionLayers_Enemy_Bullet);
 		collisionComp->SetCollisionMask(CollisionLayers_Player);
 	}

 	collisionComp->SetSize(glm::vec2(16.0f));

 	// 測試
 	auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
 	currentScene->GetRoot().lock()->AddChild(collisionComp->GetVisibleBox());
 	currentScene->GetCamera().lock()->AddChild(collisionComp->GetVisibleBox());
 }

void EffectAttack::UpdateObject(float deltaTime) {
	if (!m_Active) return;
	if (m_animation->IfAnimationEnds()) {
		SetActive(false);
		const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
		scene->GetRoot().lock()->RemoveChild(shared_from_this());
		scene->GetCamera().lock()->RemoveChild(shared_from_this());
		scene->GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION)->UnregisterNGameObject(shared_from_this());
	}
}


void EffectAttack::ResetAll(const CharacterType type, const Util::Transform &attackTransform, glm::vec2 direction,
							float size, int damage, bool canReflect, EffectAttackType effectType)
{
	m_type = type;
 	this->m_Transform = attackTransform;
 	m_direction = direction;
 	m_size = size;
 	m_damage = damage;
 	m_effectType = effectType;
	m_reflectBullet = canReflect;
 	auto& paths = EffectAssets::EFFECT_IMAGE_PATHS.at(effectType);
 	m_animation = std::make_shared<Animation>(paths, false);
}

void EffectAttack::onCollision(const std::shared_ptr<nGameObject> &other, CollisionInfo &info)
{
	//TODO
}

