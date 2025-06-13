//
// Created by tjx20 on 4/19/2025.
//

#include "Attack/Attack.hpp"

#include "Attack/AttackManager.hpp"
#include "RandomUtil.hpp"
#include "Scene/SceneManager.hpp"

Attack::Attack(const AttackInfo &attack_info) :
	m_type(attack_info.type), m_direction(attack_info.direction), m_size(attack_info.size),
	m_damage(attack_info.damage), m_elementalDamage(attack_info.elementalDamage),
	m_chainAttack(attack_info.chainAttack), m_isCriticalHit(attack_info.isCriticalHit)
{
	this->m_Transform = attack_info.attackTransform;
}

void Attack::TriggerChainAttack() const
{
	auto chainAttackInfo = this->GetChainAttackInfo();
	if (!chainAttackInfo.enabled)
		return;
	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!currentScene)
		return;
	const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	if (!attackManager)
		return;

	if (chainAttackInfo.attackType == AttackType::EFFECT_ATTACK)
	{
		const auto effectInfoPtr = std::dynamic_pointer_cast<EffectAttackInfo>(chainAttackInfo.nextAttackInfo);
		if (!effectInfoPtr)
			return;
		effectInfoPtr->type = this->GetAttackLayerType();
		effectInfoPtr->attackTransform = this->GetTransform();
		// 重新複製並更新 nextAttackInfo
		if (effectInfoPtr->continuouslyExtending)
		{
			auto offset = this->m_direction * this->m_size;
			effectInfoPtr->attackTransform.translation = this->GetWorldCoord() + offset;
			effectInfoPtr->direction = this->m_direction;

			effectInfoPtr->chainAttack.enabled = true;
			effectInfoPtr->chainAttack.attackType = AttackType::EFFECT_ATTACK;
			// 淺複製一份新的 EffectAttackInfo
			auto copyAttack = std::make_shared<EffectAttackInfo>(*effectInfoPtr);
			// 避免遞迴爆炸：重設 chainAttack
			copyAttack->chainAttack = {}; // 或 clear 掉
			effectInfoPtr->chainAttack.nextAttackInfo = copyAttack;
		}
		else
		{
			effectInfoPtr->attackTransform.translation = this->GetWorldCoord();
			effectInfoPtr->attackTransform.rotation = 0.0f;
			effectInfoPtr->direction = glm::vec2(0.0f);
		}

		effectInfoPtr->attackTransform.scale = glm::vec2(1.0f);
		attackManager->spawnEffectAttack(*effectInfoPtr);
	}
	else if (chainAttackInfo.attackType == AttackType::PROJECTILE)
	{
		const auto projInfoPtr = std::dynamic_pointer_cast<ProjectileInfo>(chainAttackInfo.nextAttackInfo);
		if (!projInfoPtr)
			return;

		const int numBullets = projInfoPtr->chainProjectionNum;
		if (numBullets <= 0)
			return;
		ProjectileInfo baseInfo = *projInfoPtr; // 淺複製一次

		baseInfo.type = this->GetAttackLayerType();
		baseInfo.attackTransform = this->GetTransform();
		baseInfo.attackTransform.translation = this->GetWorldCoord();
		baseInfo.attackTransform.scale = glm::vec2(1.0f);
		baseInfo.direction = this->m_direction;

		for (int i = 0; i < numBullets; ++i)
		{
			float angleInRadians = glm::radians((360.0f / numBullets) * i);
			glm::vec2 newDirection = glm::vec2(cos(angleInRadians), sin(angleInRadians));

			baseInfo.attackTransform.rotation = angleInRadians;
			baseInfo.direction = newDirection;

			attackManager->spawnProjectile(baseInfo);
		}
	}
	else
		LOG_ERROR("error attackType");
}
