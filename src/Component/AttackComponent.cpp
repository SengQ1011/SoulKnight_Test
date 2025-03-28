//
// Created by tjx20 on 3/26/2025.
//

#include "Components/AttackComponent.hpp"

#include "Creature/Character.hpp"

AttackComponent::AttackComponent(float criticalRate, int handBlademage, std::shared_ptr<Weapon> initWeapon)
	: m_criticalRate(criticalRate), m_maxWeapon(2), m_handBladeDamage(handBlademage),m_currentWeapon(initWeapon){}

void AttackComponent::Init()
{
	AddWeapon(m_currentWeapon);
	auto character = GetOwner<Character>();
	if(character)
	{
		character->AddChild(m_currentWeapon);
	}
	// LOG_DEBUG("{}",m_currentWeapon->GetImagePath());
	// LOG_DEBUG("Weapon System && Attack Function Init");
}

void AttackComponent::Update()
{
	float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	// Weapon中的followerComponent更新
	m_currentWeapon->Update();
	m_currentWeapon->UpdateCooldown(deltaTime);
}


void AttackComponent::AddWeapon(std::shared_ptr<Weapon> newWeapon) {
	if (m_Weapons.size() >= m_maxWeapon) {
		RemoveWeapon();  // 刪除舊武器
	}

	m_Weapons.push_back(newWeapon); // 添加新武器
	m_currentWeapon = newWeapon; // 更新當前武器
}


void AttackComponent::RemoveWeapon() {
	auto it = std::find_if(m_Weapons.begin(), m_Weapons.end(),
						[&](const std::shared_ptr<Weapon>& weapon) { return weapon == m_currentWeapon; });
	if (it != m_Weapons.end()) {
		m_Weapons.erase(it);
	}
}

void AttackComponent::switchWeapon() {
	if (m_Weapons.empty()) return;

	auto it = std::find(m_Weapons.begin(), m_Weapons.end(), m_currentWeapon);
	if (it != m_Weapons.end() && std::next(it) != m_Weapons.end()) {
		auto
		m_currentWeapon = *std::next(it);
	} else {
		m_currentWeapon = m_Weapons.front();  // 循環回到第一把武器
	}
}

int AttackComponent::calculateDamage()
{
	// 計算總暴擊率
	float totalCritRate = m_criticalRate + m_currentWeapon->GetCriticalRate();

	// 限制暴擊率在0-100%之間
	totalCritRate = std::clamp(totalCritRate, 0.0f, 1.0f);

	// 是否觸發暴擊
	bool isCrit = (rand() / static_cast<float>(RAND_MAX)) < totalCritRate;

	// 傷害計算公式
	int baseDamage = m_currentWeapon->GetDamage();
	int finalDamage = isCrit
		? baseDamage * (2)  // 暴擊傷害==>200%
		: baseDamage;  // 普通傷害

	return finalDamage;

}

void AttackComponent::TryAttack() {
	if (m_currentWeapon && m_currentWeapon->CanAttack()) {
		auto damage = calculateDamage();
		m_currentWeapon->attack(damage);
	}
}