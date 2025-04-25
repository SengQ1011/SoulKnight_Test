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
	// 初始化切換武器時間
	m_switchTimeCounter = m_switchCooldown;
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
	// scene->GetRoot().lock()->AddChild(m_currentWeapon);
	// scene->GetCamera().lock()->AddChild(m_currentWeapon);
	scene->GetPendingObjects().push_back(m_currentWeapon); //暂时存入一次过设置渲染和镜头
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
	// 更新切換冷卻時間
	m_switchTimeCounter -= deltaTime;
}


void AttackComponent::AddWeapon(const std::shared_ptr<Weapon>& newWeapon)
{
	auto character = GetOwner<Character>();
	if (!character)
		return;
	if (m_Weapons.size() >= m_maxWeapon)
	{
		RemoveWeapon(); // 刪除舊武器
	}
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	scene->GetRoot().lock()->RemoveChild(m_currentWeapon);
	scene->GetCamera().lock()->RemoveChild(m_currentWeapon);

	m_Weapons.push_back(newWeapon); // 添加新武器列表
	m_currentWeapon = newWeapon; // 更新當前武器
	m_currentWeapon->SetOwner(character); // 當前武器添加擁有者的指標
	if(auto followerComp = m_currentWeapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
	{
		followerComp->SetFollower(character);
	}

	// scene->GetRoot().lock()->AddChild(m_currentWeapon);
	// scene->GetCamera().lock()->AddChild(m_currentWeapon);
	scene->GetPendingObjects().push_back(m_currentWeapon);
}


void AttackComponent::RemoveWeapon()
{
	auto it = std::find_if(m_Weapons.begin(), m_Weapons.end(),
						   [&](const std::shared_ptr<Weapon> &weapon) { return weapon == m_currentWeapon; });
	if (it != m_Weapons.end())
	{
		it->get()->RemoveOwner();
		if (auto followerComp = it->get()->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
		{
			followerComp->SetFollower(nullptr);
		}
		m_Weapons.erase(it);
	}
}

void AttackComponent::switchWeapon()
{
	if (m_switchTimeCounter > 0)
		return;
	if (m_Weapons.empty())
		return;

	// 重置冷卻時間
	m_switchTimeCounter = m_switchCooldown;

	// OpenGL 本身是「狀態機 + 立即模式」，它不會自己管理任何物件，它只會畫你給它的東西
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	scene->GetRoot().lock()->RemoveChild(m_currentWeapon);
	scene->GetCamera().lock()->RemoveChild(m_currentWeapon);

	// 找目前武器的位置
	auto it = std::find(m_Weapons.begin(), m_Weapons.end(), m_currentWeapon);
	if (it != m_Weapons.end() && std::next(it) != m_Weapons.end()) {
		m_currentWeapon = *std::next(it);
	} else {
		m_currentWeapon = m_Weapons.front(); // 循環回到第一把武器
	}
	scene->GetRoot().lock()->AddChild(m_currentWeapon);
	scene->GetCamera().lock()->AddChild(m_currentWeapon);
	// scene->GetPendingObjects().push_back(m_currentWeapon);
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

	const auto healthComponent = character->GetComponent<HealthComponent>(ComponentType::HEALTH);
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
				const auto damage = calculateDamage();
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
		// scene->GetRoot().lock()->AddChild(m_secondWeapon);
		// scene->GetCamera().lock()->AddChild(m_secondWeapon);
		scene->GetPendingObjects().push_back(m_secondWeapon);
	}
}

void AttackComponent::OnEnemyPositionUpdate(std::weak_ptr<Character> enemy) {
	if (auto locked = enemy.lock()) {
		this->SetTarget(std::static_pointer_cast<nGameObject>(locked));
		// 通知目前武器最靠近的目標
		if (const auto followerComp = m_currentWeapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER)) {
			followerComp->SetTarget(std::static_pointer_cast<nGameObject>(locked));
		}
		// 若有雙武器也通知
		if(m_dualWield) {
			if (const auto followerComp2 = m_secondWeapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER)) {
				followerComp2->SetTarget(std::static_pointer_cast<nGameObject>(locked));
			}
		}
	}
}

void AttackComponent::OnLostEnemy() {
	m_Target.reset();
	for(auto& weapon : m_Weapons) {
		if (const auto followerComp = weapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER)) {
			followerComp->SetTarget(nullptr);
		}
	}

	if(m_dualWield) {
		if (const auto followerComp2 = m_secondWeapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER)) {
			followerComp2->SetTarget(nullptr);
		}
	}
}