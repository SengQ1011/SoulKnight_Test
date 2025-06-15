//
// Created by Assistant on 2025/1/20.
//

#include "DestructionEffects/IceSpikeEffect.hpp"
#include <glm/gtc/constants.hpp>
#include "Attack/AttackManager.hpp"
#include "Attack/Projectile.hpp"
#include "RandomUtil.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Logger.hpp"


IceSpikeEffect::IceSpikeEffect(int spikeCount, int spikeDamage, float spikeSpeed) :
	m_spikeCount(spikeCount), m_spikeDamage(spikeDamage), m_spikeSpeed(spikeSpeed)
{
}

void IceSpikeEffect::Execute(const glm::vec2 &position, CharacterType attackerType)
{
	try
	{
		const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
		if (!currentScene)
		{
			LOG_ERROR("Cannot get current scene for ice spike effect");
			return;
		}

		const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
		if (!attackManager)
		{
			LOG_ERROR("Cannot get attack manager for ice spike effect");
			return;
		}

		// 發射多個方向的冰刺
		for (int i = 0; i < m_spikeCount; ++i)
		{
			float angle = (360.0f / m_spikeCount) * i;
			glm::vec2 direction = glm::vec2(cos(glm::radians(angle)), sin(glm::radians(angle)));

			// 創建冰刺子彈
			ProjectileInfo iceSpike;
			iceSpike.type = CharacterType::NEUTRAL; // 使用中性類型，不分敵我
			iceSpike.attackTransform.translation = position;
			iceSpike.attackTransform.rotation = glm::radians(angle + 90.0f); // 左旋轉90度, 照片問題
			iceSpike.attackTransform.scale = glm::vec2(1.0f);
			iceSpike.direction = direction;
			iceSpike.imagePath = RESOURCE_DIR "/attackUI/cave_ice_nail_1.png"; // 使用現有的冰彈圖片
			iceSpike.size = 12.0f;
			iceSpike.damage = m_spikeDamage;
			iceSpike.speed = m_spikeSpeed;
			iceSpike.elementalDamage = StatusEffect::FROZEN;
			iceSpike.canReboundBySword = false;
			iceSpike.canTracking = false;
			iceSpike.isBubble = false;
			iceSpike.bubbleTrail = false;
			iceSpike.numRebound = 0;

			// 生成冰刺子彈
			attackManager->spawnProjectile(iceSpike);
		}

		//LOG_DEBUG("Ice spike effect triggered at position ({}, {}) with {} spikes", position.x, position.y,m_spikeCount);
	}
	catch (const std::exception &e)
	{
		LOG_ERROR("Exception in IceSpikeEffect::Execute: {}", e.what());
	}
}
