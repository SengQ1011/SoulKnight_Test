//
// Created by tjx20 on 4/19/2025.
//

#include "Attack/EffectAttack.hpp"
#include "Animation.hpp"
#include "Components/CollisionComponent.hpp"
#include "ObserveManager/EventManager.hpp"
#include "Room/RoomCollisionManager.hpp"
#include "Scene/SceneManager.hpp"
#include "TriggerStrategy/AttackTriggerStrategy.hpp"
#include "TriggerStrategy/BlockProjectileStrategy.hpp"
#include "TriggerStrategy/KnockOffTriggerStrategy.hpp"
#include "TriggerStrategy/ReflectTriggerStrategy.hpp"

EffectAttack::EffectAttack(const EffectAttackInfo &effectAttackInfo) :
	Attack(effectAttackInfo), m_reflectBullet(effectAttackInfo.canReflectBullet),
	m_bulletBlocking(effectAttackInfo.canBlockingBullet), m_effectType(effectAttackInfo.effectType)
{
}

void EffectAttack::Init()
{
	// 明確設定世界坐標（從傳入的 Transform 取得）
	this->m_WorldCoord = m_Transform.translation;

	// Animation
	const auto &imagePaths = EffectAssets::EFFECT_IMAGE_PATHS.at(m_effectType);
	float interval = 400 / imagePaths.size();
	m_animation = std::make_shared<Animation>(imagePaths, false, "EffectAttack", interval);
	this->SetDrawable(m_animation->GetDrawable());
	m_animation->PlayAnimation(true);

	// TODO: 這裏可以work，但是 @錦鑫你看一下怎麽放比較舒服
	if (m_effectType == EffectAttackType::MEDIUM_BOOM)
	{
		EventManager::TriggerCameraShake(0.5, 15);
	}

	// 只初始化碰撞組件，不加入渲染器
	auto CollisionComp = this->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (!CollisionComp)
	{
		CollisionComp = this->AddComponent<CollisionComponent>(ComponentType::COLLISION);
	}
	CollisionComp->ResetCollisionMask();

	// 設置觸發器 和 觸發事件
	CollisionComp->ClearTriggerStrategies();
	CollisionComp->ClearTriggerTargets();
	CollisionComp->SetTrigger(true);
	CollisionComp->SetCollider(false);
	CollisionComp->AddTriggerStrategy(std::make_unique<AttackTriggerStrategy>(m_damage, m_elementalDamage));
	if (m_effectType == EffectAttackType::SHOCKWAVE)
		CollisionComp->AddTriggerStrategy(std::make_unique<KnockOffTriggerStrategy>(m_shockwaveForce));
	if (m_type == CharacterType::PLAYER)
	{
		if (m_reflectBullet)
			CollisionComp->AddTriggerStrategy(std::make_unique<ReflectTriggerStrategy>());
		else
			CollisionComp->AddTriggerStrategy(std::make_unique<BlockProjectileStrategy>());
	}

	// layer
	if (m_type == CharacterType::PLAYER)
	{
		CollisionComp->SetCollisionLayer(CollisionLayers::CollisionLayers_Player_EffectAttack);
		CollisionComp->AddCollisionMask(CollisionLayers::CollisionLayers_Enemy);
		CollisionComp->AddCollisionMask(CollisionLayers::CollisionLayers_DestructibleTerrain);
		if (m_bulletBlocking)
			CollisionComp->AddCollisionMask(CollisionLayers::CollisionLayers_Enemy_Projectile);
	}
	else if (m_type == CharacterType::ENEMY)
	{
		CollisionComp->SetCollisionLayer(CollisionLayers::CollisionLayers_Enemy_EffectAttack);
		CollisionComp->AddCollisionMask(CollisionLayers::CollisionLayers_Player);
		CollisionComp->AddCollisionMask(CollisionLayers::CollisionLayers_DestructibleTerrain);
	}

	CollisionComp->SetSize(glm::vec2(m_size));

	// TODO測試
	// const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	// 	currentScene->GetRoot().lock()->AddChild(CollisionComp->GetVisibleBox());
	// 	currentScene->GetCamera().lock()->SafeAddChild(CollisionComp->GetVisibleBox());
}

void EffectAttack::UpdateObject(const float deltaTime)
{
	if (!m_Active)
		return;
	if (m_animation->IfAnimationEnds())
	{
		MarkForRemoval();
		TriggerChainAttack();
		SetActive(false);
		const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
		scene->GetRoot().lock()->RemoveChild(shared_from_this());
		// scene->GetCamera().lock()->RemoveChild(shared_from_this());
		scene->GetCamera().lock()->MarkForRemoval(shared_from_this());
		scene->GetCurrentRoom()
			->GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION)
			->UnregisterNGameObject(shared_from_this());
	}
}


void EffectAttack::ResetAll(const EffectAttackInfo &effectAttackInfo)
{
	m_type = effectAttackInfo.type;
	m_Transform = effectAttackInfo.attackTransform;
	m_direction = effectAttackInfo.direction;
	m_size = effectAttackInfo.size;
	m_damage = effectAttackInfo.damage;
	m_elementalDamage = effectAttackInfo.elementalDamage;
	m_chainAttack = effectAttackInfo.chainAttack;

	m_effectType = effectAttackInfo.effectType;
	m_reflectBullet = effectAttackInfo.canReflectBullet;
	m_bulletBlocking = effectAttackInfo.canBlockingBullet;
	auto &paths = EffectAssets::EFFECT_IMAGE_PATHS.at(effectAttackInfo.effectType);
	m_animation = std::make_shared<Animation>(paths, false);
	this->m_markRemove = false;
}
