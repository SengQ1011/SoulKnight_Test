//
// Created by tjx20 on 4/19/2025.
//

#include "Attack/EffectAttack.hpp"
#include "Animation.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/EffectAttackComponent.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "ObserveManager/EventManager.hpp"
#include "Room/RoomCollisionManager.hpp"
#include "Scene/SceneManager.hpp"
#include "TriggerStrategy/AttackTriggerStrategy.hpp"
#include "TriggerStrategy/BlockProjectileStrategy.hpp"
#include "TriggerStrategy/KnockOffTriggerStrategy.hpp"
#include "TriggerStrategy/ReflectTriggerStrategy.hpp"

EffectAttack::EffectAttack(const EffectAttackInfo &effectAttackInfo) :
	Attack(effectAttackInfo), m_effectType(effectAttackInfo.effectType),
	m_reflectBullet(effectAttackInfo.canReflectBullet), m_bulletBlocking(effectAttackInfo.canBlockingBullet),
	m_continuouslyExtending(effectAttackInfo.continuouslyExtending),
	m_intervalCreateChainAttack(effectAttackInfo.intervalCreateChainAttack),
	m_timer(effectAttackInfo.intervalCreateChainAttack)
{
}

void EffectAttack::Init()
{
	// 明確設定世界坐標（從傳入的 Transform 取得）
	this->m_WorldCoord = m_Transform.translation;

	// 播放對應的音效
	switch (m_effectType)
	{
	case EffectAttackType::SHOCKWAVE:
	case EffectAttackType::LARGE_SHOCKWAVE:
		AudioManager::GetInstance().PlaySFX("hit_floor_sound");
		break;
	case EffectAttackType::LARGE_BOOM:
	case EffectAttackType::MEDIUM_BOOM:
		AudioManager::GetInstance().PlaySFX("explode_big");
		break;
	case EffectAttackType::SMALL_BOOM:
		AudioManager::GetInstance().PlaySFX("explode_small");
		break;
	default:
		break;
	}

	// Animation
	const auto &imagePaths = EffectAssets::EFFECT_IMAGE_PATHS.at(m_effectType);
	float interval = 400 / imagePaths.size();
	if (m_effectType == EffectAttackType::LARGE_SHOCKWAVE)
		interval *= 2.0f;
	else if (m_effectType == EffectAttackType::POISON_AREA)
		interval = 4000 / imagePaths.size();

	m_animation = std::make_shared<Animation>(imagePaths, false, interval, "Animation");
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
	CollisionComp->AddTriggerStrategy(
		std::make_unique<AttackTriggerStrategy>(m_damage, m_elementalDamage, m_isCriticalHit));
	if (m_effectType == EffectAttackType::SHOCKWAVE || m_effectType == EffectAttackType::LARGE_SHOCKWAVE)
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
	else
	{
		CollisionComp->SetCollisionLayer(CollisionLayers::CollisionLayers_Player_EffectAttack);
		CollisionComp->AddCollisionLayer(CollisionLayers::CollisionLayers_Enemy_EffectAttack); // 偷吃步
		CollisionComp->AddCollisionMask(CollisionLayers::CollisionLayers_Player);
		CollisionComp->AddCollisionMask(CollisionLayers::CollisionLayers_Enemy);
		CollisionComp->AddCollisionMask(CollisionLayers::CollisionLayers_DestructibleTerrain);
	}

	if (m_effectType == EffectAttackType::ICE_SPIKE)
	{
		CollisionComp->AddCollisionMask(CollisionLayers::CollisionLayers_Terrain);
		if (auto EffectAttackComp = this->AddComponent<EffectAttackComponent>(ComponentType::EFFECT_ATTACK))
		{
			CollisionComp->SetCollider(true);
		}
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

	if (m_continuouslyExtending && !m_collisionWithTerrain)
	{
		m_timer -= deltaTime;
		if (m_timer <= 0)
		{
			TriggerChainAttack();
			this->m_chainAttack = {};
		}
	}
	if (m_animation->IfAnimationEnds())
	{
		if (!m_continuouslyExtending)
			TriggerChainAttack();
		MarkForRemoval();
		SetActive(false);
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
	m_collisionWithTerrain = false;

	m_continuouslyExtending = effectAttackInfo.continuouslyExtending;
	m_intervalCreateChainAttack = effectAttackInfo.intervalCreateChainAttack;
	m_timer = effectAttackInfo.intervalCreateChainAttack;

	auto &paths = EffectAssets::EFFECT_IMAGE_PATHS.at(effectAttackInfo.effectType);
	m_animation = std::make_shared<Animation>(paths, false, 0, "Animation");
	this->m_markRemove = false;

	if (this->RemoveComponent<EffectAttackComponent>(ComponentType::EFFECT_ATTACK))
	{
		// 組件成功移除
		// LOG_DEBUG("remove");
	}
}
