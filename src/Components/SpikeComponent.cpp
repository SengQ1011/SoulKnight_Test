//
// Created by tjx20 on 6/12/2025.
//

#include "Components\SpikeComponent.hpp"

#include <utility>

#include "Override/nGameObject.hpp"

SpikeComponent::SpikeComponent(std::vector<std::string> imagePaths, const int damage)
	 : Component(ComponentType::SPIKE), m_imagePaths(std::move(imagePaths)), m_damage(damage){}

void SpikeComponent::Init()
{
	auto& imagePoolManager = ImagePoolManager::GetInstance();
	for (auto& imagePath : m_imagePaths)
	{
		const auto drawable = imagePoolManager.GetImage(RESOURCE_DIR + imagePath);
		m_drawables.emplace_back(drawable);
	}
	const std::shared_ptr<nGameObject> spike = GetOwner<nGameObject>();
	if (!spike) return;
	spike->SetActive(true);
	spike->SetControlVisible(true);
	spike->SetDrawable(m_drawables[0]);
	m_spikeState = spikeState::HIDDEN;
	m_hiddenTimer.SetAndStart(3.0f);
}

void SpikeComponent::Update() {

 	if (m_spikeState == spikeState::HIDDEN) {
 		m_hiddenTimer.Update();
 		if (m_hiddenTimer.IsFinished()) {
 			LOG_DEBUG("check");
 			m_spikeState = spikeState::ACTIVE;
 			m_activeTimer.SetAndStart(2.0f); // 弹出2秒
			if (const auto owner = GetOwner<nGameObject>())
			{
				owner->SetDrawable(m_drawables[1]);
				LOG_DEBUG("change to active");
				if (const auto collisionComp = owner->GetComponent<CollisionComponent>(ComponentType::COLLISION))
				{
					collisionComp->SetActive(true);
					collisionComp->AddTriggerStrategy(std::make_unique<AttackTriggerStrategy>(m_damage, StatusEffect::NONE));
				}
			}
 		}
 	} else if (m_spikeState == spikeState::ACTIVE) {
 		m_activeTimer.Update();
 		if (m_activeTimer.IsFinished()) {
 			m_spikeState = spikeState::HIDDEN;
 			m_hiddenTimer.SetAndStart(3.0f); // 收回3秒
 			if (const auto owner = GetOwner<nGameObject>())
 			{
 				owner->SetDrawable(m_drawables[0]);
 				LOG_DEBUG("change to hidden");
 				if (const auto collisionComp = owner->GetComponent<CollisionComponent>(ComponentType::COLLISION))
 				{
 					collisionComp->SetActive(false);
 					collisionComp->ClearTriggerStrategies();
 					collisionComp->ClearTriggerTargets();
 				}
 			}
 		}
 	}
 }
