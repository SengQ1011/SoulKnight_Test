//
// Created by tjx20 on 3/26/2025.
//

#include "Components/AttackComponent.hpp"
#include "Scene/SceneManager.hpp"
#include "Components/HealthComponent.hpp"
#include "Creature/Character.hpp"

AttackComponent::AttackComponent(std::shared_ptr<Weapon> initWeapon = nullptr, float criticalRate = 0, int handBladeDamage = 0, int colLisionDamage = 0)
	: m_criticalRate(criticalRate), m_handBladeDamage(handBladeDamage), m_collisionDamage(colLisionDamage),m_currentWeapon(initWeapon){}

void AttackComponent::Init()
{
	// 每個角色都會武器，除了部分小怪
	if (!m_currentWeapon) return;
	AddWeapon(m_currentWeapon);
	// 武器記錄擁有者
	auto character = GetOwner<Character>();
	m_currentWeapon->SetOwner(character);

	if(character->GetType() == CharacterType::ENEMY) {
		SetMaxWeapon(1);
	} else if(character->GetType() == CharacterType::PLAYER) {
		SetMaxWeapon(2);
	}

	// 加入渲染樹
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if(!scene)return;
	scene->GetRoot().lock()->AddChild(m_currentWeapon);
	scene->GetCamera().lock()->AddRelativePivotChild(m_currentWeapon);
}

void AttackComponent::Update()
{
	float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	// Weapon中的followerComponent更新
	if(!m_currentWeapon) return;
	m_currentWeapon->Update();
	m_currentWeapon->UpdateCooldown(deltaTime);
}


void AttackComponent::AddWeapon(std::shared_ptr<Weapon> newWeapon) {
	auto character = GetOwner<Character>();
	if(!character) return;
	if (m_Weapons.size() >= m_maxWeapon) {
		RemoveWeapon();  // 刪除舊武器
	}

	m_Weapons.push_back(newWeapon);			// 添加新武器列表
	m_currentWeapon = newWeapon;			// 更新當前武器
	m_currentWeapon->SetOwner(character);	// 當前武器添加擁有者的指標
}


void AttackComponent::RemoveWeapon() {
	auto it = std::find_if(m_Weapons.begin(), m_Weapons.end(),
						[&](const std::shared_ptr<Weapon>& weapon) { return weapon == m_currentWeapon; });
	if (it != m_Weapons.end()) {
		it->get()->RemoveOwner();
		m_Weapons.erase(it);
	}
}

void AttackComponent::switchWeapon() {
	if (m_Weapons.empty()) return;
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();

	auto it = std::find(m_Weapons.begin(), m_Weapons.end(), m_currentWeapon);
	if (it != m_Weapons.end() && std::next(it) != m_Weapons.end()) {
		auto m_currentWeapon = *std::next(it);
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
	auto character = GetOwner<Character>();
	if (!character) return;

	auto healthComponent = character->GetComponent<HealthComponent>(ComponentType::HEALTH);
	auto currentEnergy = healthComponent->GetCurrentEnergy();

	if (m_currentWeapon && m_currentWeapon->CanAttack() && currentEnergy > 0) {
		auto damage = calculateDamage();
		m_currentWeapon->attack(damage);

		if (character->GetType() == CharacterType::PLAYER) {
			healthComponent->ConsumeEnergy(m_currentWeapon->GetEnergy());
		}
	}
}

// TODO:技能：火力全開（雙武器）
void AttackComponent::SetDualWield(const bool enable) {
	if (enable) {

	}
	else
	{

	}
}