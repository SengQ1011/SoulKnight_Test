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

		// 創建毒圈特效
		// 注意：這裡假設有 POISON_CLOUD 效果類型，如果沒有可以用 SMALL_BOOM 替代
		EffectAttackInfo poisonCloud;
		poisonCloud.type = CharacterType::NEUTRAL; // 使用中性類型，不分敵我
		poisonCloud.attackTransform.translation = position;
		poisonCloud.attackTransform.scale = glm::vec2(1.0f);
		poisonCloud.attackTransform.rotation = 0.0f;
		poisonCloud.direction = glm::vec2(0.0f);
		poisonCloud.size = m_cloudSize;
		poisonCloud.damage = m_poisonDamage;
		poisonCloud.elementalDamage = StatusEffect::POISON;

		// 暫時使用 SMALL_BOOM，之後可以新增 POISON_CLOUD 類型
		poisonCloud.effectType = EffectAttackType::SMALL_BOOM;
		poisonCloud.canReflectBullet = false;
		poisonCloud.canBlockingBullet = false;

		// 生成毒圈特效
		attackManager->spawnEffectAttack(poisonCloud);

		LOG_DEBUG("Poison cloud effect triggered at position ({}, {}) with size {}", position.x, position.y,
				  m_cloudSize);

		// TODO: 實現持續時間效果
		// 可以考慮創建多個小的毒雲效果來模擬持續傷害
	}
	catch (const std::exception &e)
	{
		LOG_ERROR("Exception in PoisonCloudEffect::Execute: {}", e.what());
	}
}
