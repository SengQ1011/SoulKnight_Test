//
// Created by Assistant on 2025/1/20.
//

#include "DestructionEffects/ExplosionEffect.hpp"
#include "Attack/AttackManager.hpp"
#include "Attack/EffectAttack.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Logger.hpp"

ExplosionEffect::ExplosionEffect(float explosionSize, int explosionDamage) :
	m_explosionSize(explosionSize), m_explosionDamage(explosionDamage)
{
}

void ExplosionEffect::Execute(const glm::vec2 &position, CharacterType attackerType)
{
	try
	{
		const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
		if (!currentScene)
		{
			LOG_ERROR("Cannot get current scene for explosion effect");
			return;
		}

		const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
		if (!attackManager)
		{
			LOG_ERROR("Cannot get attack manager for explosion effect");
			return;
		}

		// 創建爆炸特效
		EffectAttackInfo explosionInfo;
		explosionInfo.type = CharacterType::NEUTRAL ; // 使用中性類型，不分敵我
		explosionInfo.attackTransform.translation = position;
		explosionInfo.attackTransform.scale = glm::vec2(1.0f);
		explosionInfo.attackTransform.rotation = 0.0f;
		explosionInfo.direction = glm::vec2(0.0f);
		explosionInfo.size = m_explosionSize;
		explosionInfo.damage = m_explosionDamage;
		explosionInfo.elementalDamage = StatusEffect::BURNS;
		explosionInfo.effectType = EffectAttackType::MEDIUM_BOOM;
		explosionInfo.canReflectBullet = false;
		explosionInfo.canBlockingBullet = false;

		// 生成爆炸特效
		attackManager->spawnEffectAttack(explosionInfo);

		LOG_DEBUG("Explosion effect triggered at position ({}, {})", position.x, position.y);
	}
	catch (const std::exception &e)
	{
		LOG_ERROR("Exception in ExplosionEffect::Execute: {}", e.what());
	}
}
