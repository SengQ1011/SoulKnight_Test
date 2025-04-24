//
// Created by tjx20 on 3/26/2025.
//

#include "Components/AttackComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Creature/Character.hpp"
#include "Scene/SceneManager.hpp"
#include "Weapon/Weapon.hpp"


AttackComponent::AttackComponent(const std::shared_ptr<Weapon> &initWeapon = nullptr, const float criticalRate = 0,
								 const int handBladeDamage = 0, const int collisionDamage = 0) :
	m_criticalRate(criticalRate), m_handBladeDamage(handBladeDamage), m_collisionDamage(collisionDamage),
	m_currentWeapon(initWeapon)
{
}

void AttackComponent::Init()
{
	// 每個角色都會武器，除了部分小怪
	if (!m_currentWeapon)
		return;
	AddWeapon(m_currentWeapon);
	// 武器記錄擁有者
	auto character = GetOwner<Character>();
	m_currentWeapon->SetOwner(character);

	if (character->GetType() == CharacterType::ENEMY)
	{
		SetMaxWeapon(1);
	}
	else if (character->GetType() == CharacterType::PLAYER)
	{
		SetMaxWeapon(2);
	}

	// 加入渲染樹
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene)
		return;
	scene->GetRoot().lock()->AddChild(m_currentWeapon);
	scene->GetCamera().lock()->AddChild(m_currentWeapon);
}

void AttackComponent::Update()
{
	float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	// Weapon中的followerComponent更新
	if (m_currentWeapon)
	{
		m_currentWeapon->Update();
		m_currentWeapon->UpdateCooldown(deltaTime);
	}
	if (m_secondWeapon)
	{
		m_secondWeapon->Update();
		m_secondWeapon->UpdateCooldown(deltaTime);
	}
}


void AttackComponent::AddWeapon(std::shared_ptr<Weapon> newWeapon)
{
	auto character = GetOwner<Character>();
	if (!character)
		return;
	if (m_Weapons.size() >= m_maxWeapon)
	{
		RemoveWeapon(); // 刪除舊武器
	}

	m_Weapons.push_back(newWeapon); // 添加新武器列表
	m_currentWeapon = newWeapon; // 更新當前武器
	m_currentWeapon->SetOwner(character); // 當前武器添加擁有者的指標
}


void AttackComponent::RemoveWeapon()
{
	auto it = std::find_if(m_Weapons.begin(), m_Weapons.end(),
						   [&](const std::shared_ptr<Weapon> &weapon) { return weapon == m_currentWeapon; });
	if (it != m_Weapons.end())
	{
		it->get()->RemoveOwner();
		m_Weapons.erase(it);
	}
}

void AttackComponent::switchWeapon()
{
	if (m_Weapons.empty())
		return;
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();

	auto it = std::find(m_Weapons.begin(), m_Weapons.end(), m_currentWeapon);
	if (it != m_Weapons.end() && std::next(it) != m_Weapons.end())
	{
		auto m_currentWeapon = *std::next(it);
	}
	else
	{
		m_currentWeapon = m_Weapons.front(); // 循環回到第一把武器
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
	int finalDamage = isCrit ? baseDamage * (2) // 暴擊傷害==>200%
							 : baseDamage; // 普通傷害

	return finalDamage;
}

void AttackComponent::TryAttack()
{
	auto character = GetOwner<Character>();
	if (!character)
		return;

	auto healthComponent = character->GetComponent<HealthComponent>(ComponentType::HEALTH);
	if (!healthComponent)
		return;
	const bool isPlayer = (character->GetType() == CharacterType::PLAYER);
	const float currentEnergy = healthComponent->GetCurrentEnergy();

	if (isPlayer && currentEnergy <= 0)
	{
		LOG_DEBUG("AttackComponent: Not enough energy to attack");
		return;
	}

	// 主武器攻击
	if (m_currentWeapon && m_currentWeapon->CanAttack())
	{
		if (currentEnergy >= m_currentWeapon->GetEnergy())
		{
			auto damage = calculateDamage();
			m_currentWeapon->attack(damage);

			if (isPlayer)
			{
				healthComponent->ConsumeEnergy(m_currentWeapon->GetEnergy());
			}
		}
	}
	// 双持模式逻辑
	if (m_dualWield)
	{
		// 确保有第二把武器
		if (!m_secondWeapon)
		{
			LOG_WARN("AttackComponent: Dual wield enabled but secondary weapon not ready");
			return;
		}
		// 副武器攻击
		if (m_dualWield && m_secondWeapon && m_secondWeapon->CanAttack())
		{
			if (currentEnergy >= m_secondWeapon->GetEnergy())
			{
				auto damage = calculateDamage();
				m_secondWeapon->attack(damage);

				if (isPlayer)
				{
					healthComponent->ConsumeEnergy(m_secondWeapon->GetEnergy());
				}
			}
		}
	}
}

// 技能：火力全開（雙武器）
void AttackComponent::SetDualWield(bool enable)
{
	m_dualWield = enable;
	if (!m_secondWeapon)
	{
		LOG_DEBUG("second weapon is nullptr");
		return;
	}
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!enable)
	{
		scene->GetRoot().lock()->RemoveChild(m_secondWeapon);
		scene->GetCamera().lock()->RemoveChild(m_secondWeapon);
		m_secondWeapon = nullptr;
	}
	else
	{
		scene->GetRoot().lock()->AddChild(m_secondWeapon);
		scene->GetCamera().lock()->AddChild(m_secondWeapon);
	}
}

