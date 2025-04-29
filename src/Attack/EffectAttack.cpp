//
// Created by tjx20 on 4/19/2025.
//

#include "Attack/EffectAttack.hpp"
#include "Scene/SceneManager.hpp"
#include "Animation.hpp"
#include "Components/CollisionComponent.hpp"
#include "Room/RoomCollisionManager.hpp"

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
	auto CollisionComp = this->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (!CollisionComp) { CollisionComp = this->AddComponent<CollisionComponent>(ComponentType::COLLISION); }
	CollisionComp->ResetCollisionMask();
	CollisionComp->SetTrigger(false);
	if(m_type == CharacterType::PLAYER) {
		CollisionComp->SetCollisionLayer(CollisionLayers::CollisionLayers_Player_Bullet);
		CollisionComp->SetCollisionMask(CollisionLayers::CollisionLayers_Enemy);
		CollisionComp->SetCollisionMask(CollisionLayers::CollisionLayers_Enemy_Bullet);
	}
	else if (m_type == CharacterType::ENEMY) {
		CollisionComp->SetCollisionLayer(CollisionLayers::CollisionLayers_Enemy_Bullet);
		CollisionComp->SetCollisionMask(CollisionLayers::CollisionLayers_Player);
	}
	CollisionComp->SetCollisionMask(CollisionLayers::CollisionLayers_Terrain);

 	CollisionComp->SetSize(glm::vec2(m_size));

 	// TODO測試
 	// auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
 	// currentScene->GetRoot().lock()->AddChild(CollisionComp->GetVisibleBox());
 	// currentScene->GetCamera().lock()->AddChild(CollisionComp->GetVisibleBox());
 }

void EffectAttack::UpdateObject(const float deltaTime) {
	if (!m_Active) return;
	if (m_animation->IfAnimationEnds()) {
		SetActive(false);
		const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
		scene->GetRoot().lock()->RemoveChild(shared_from_this());
		scene->GetCamera().lock()->RemoveChild(shared_from_this());
		scene->GetCurrentRoom()->GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION)->UnregisterNGameObject(shared_from_this());
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
	//TODO: Override OnEventReceived 讓nGameObject處理事件
}

