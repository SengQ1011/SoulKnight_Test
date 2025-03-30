//
// Created by tjx20 on 3/28/2025.
//

#include "Components/HealthComponent.hpp"

#include "Components/StateComponent.hpp"
#include "Override/nGameObject.hpp"
#include "Util/Time.hpp"

HealthComponent::HealthComponent(int maxHp, int maxArmor, int maxEnergy)
	: Component(ComponentType::HEALTH),m_maxHp(maxHp), m_currentHp(maxHp),
		m_maxArmor(maxArmor), m_currentArmor(maxArmor),
		m_maxEnergy(maxEnergy), m_currentEnergy(maxEnergy) {}

void HealthComponent::Update()
{
	float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	if(m_currentArmor < 0) {
		m_armorRecoveryTimer += deltaTime;
		if (m_armorRecoveryTimer >= m_armorRecoveryInterval) {
			AddCurrentArmor(1);
			m_armorRecoveryTimer = 0.0f;
		}
	}
}

void HealthComponent::TakeDamage(int damage)
{
	damage = std::max(0, damage - m_currentArmor);
	m_currentArmor = std::max(0, m_currentArmor - damage);
	m_currentHp = std::max(0, m_currentHp - damage);

	if (m_currentHp == 0) {
		OnDeath();
	}
}

void HealthComponent::OnDeath()
{
	auto character = GetOwner<nGameObject>();
	if(!character) return;
	auto stateComponent = character->GetComponent<StateComponent>(ComponentType::STATE);
	if(!stateComponent) return;

	stateComponent->SetState(State::DEAD);
}