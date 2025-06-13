//
// Created by Assistant on 2025/1/20.
//

#include "DestructionEffects/PoisonCloudEffect.hpp"
#include "Attack/AttackManager.hpp"
#include "Attack/EffectAttack.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Logger.hpp"

PoisonCloudEffect::PoisonCloudEffect(float cloudSize, int poisonDamage, float duration) :
	m_cloudSize(cloudSize), m_poisonDamage(poisonDamage), m_duration(duration)
{
}

void PoisonCloudEffect::Execute(const glm::vec2 &position, CharacterType attackerType)
{
	try
	{
		const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
		if (!currentScene)
		{
			LOG_ERROR("Cannot get current scene for poison cloud effect");
			return;
		}

		const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
		if (!attackManager)
		{
			LOG_ERROR("Cannot get attack manager for poison cloud effect");
			return;
		}

		// 創建毒圈特效 - 使用新的 POISON_AREA 效果類型
		EffectAttackInfo poisonCloud;
		poisonCloud.type = CharacterType::NEUTRAL; // 使用中性類型，不分敵我
		poisonCloud.attackTransform.translation = position;
		poisonCloud.attackTransform.scale = glm::vec2(1.0f);
		poisonCloud.attackTransform.rotation = 0.0f;
		poisonCloud.direction = glm::vec2(0.0f);
		poisonCloud.size = m_cloudSize;
		poisonCloud.damage = m_poisonDamage;
		poisonCloud.elementalDamage = StatusEffect::POISON;

		// 使用新的 POISON_AREA 效果類型
		poisonCloud.effectType = EffectAttackType::POISON_AREA;
		poisonCloud.canReflectBullet = false;
		poisonCloud.canBlockingBullet = false;

		// 生成毒圈特效
		attackManager->spawnEffectAttack(poisonCloud);

		LOG_DEBUG("POISON_AREA effect triggered at position ({}, {}) with size {} and damage {}", position.x,
				  position.y, m_cloudSize, m_poisonDamage);

		// POISON_AREA 效果會自動播放 4 秒的毒霧動畫
		// 16 幀動畫提供豐富的視覺效果，無需額外的持續時間處理
	}
	catch (const std::exception &e)
	{
		LOG_ERROR("Exception in PoisonCloudEffect::Execute: {}", e.what());
	}
}
