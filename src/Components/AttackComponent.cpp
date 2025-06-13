//
// Created by tjx20 on 3/26/2025.
//

#include "Components/AttackComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Creature/Character.hpp"
#include "Factory/WeaponFactory.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Scene/SceneManager.hpp"
#include "Structs/DeathEventInfo.hpp"
#include "Structs/EventInfo.hpp"
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
	m_Weapons.push_back(m_currentWeapon);
	// 武器記錄擁有者
	auto character = GetOwner<Character>();
	if (!character)
	{
		LOG_ERROR("AttackComp::character is null");
		return;
	}
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
	// scene->GetPendingObjects().push_back(m_currentWeapon); //暂时存入一次过设置渲染和镜头
	scene->GetRoot().lock()->AddChild(m_currentWeapon);
	scene->GetCamera().lock()->SafeAddChild(m_currentWeapon);
	m_currentWeapon->SetRegisteredToScene(true);
}

void AttackComponent::Update()
{
	float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	// Weapon中的followerComponent更新
	if (m_currentWeapon)
	{
		m_currentWeapon->UpdateCooldown(deltaTime);
	}
	if (m_secondWeapon)
	{
		m_secondWeapon->UpdateCooldown(deltaTime);
	}
	// 更新切換冷卻時間
	if (m_switchTimeCounter >= 0)
		m_switchTimeCounter -= deltaTime;
	if (m_pickUpWeaponTimeCounter >= 0)
		m_pickUpWeaponTimeCounter -= deltaTime;
}

std::vector<int> AttackComponent::GetAllWeaponID() const
{
	std::vector<int> id;
	for (auto &weapon : m_Weapons)
	{
		id.push_back(weapon->GetWeaponID());
	}
	return id;
}

bool AttackComponent::PickUpWeapon(const std::shared_ptr<Weapon> &newWeapon)
{
	auto character = GetOwner<Character>();
	if (!character)
		return false;
	if (m_pickUpWeaponTimeCounter > 0)
		return false;

	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	auto room = scene->GetCurrentRoom();
	if (!room)
		return false;
	auto interactableManager = room->GetInteractionManager();
	if (!interactableManager)
	{
		LOG_ERROR("AddWeapon: InteractableManager is null");
		return false;
	}

	// 先處理所有邏輯，確定成功後才取消註冊
	std::shared_ptr<Weapon> weaponToDropIfAny = nullptr;

	// 檢查是否需要丟棄舊武器
	if (m_Weapons.size() >= m_maxWeapon)
	{
		weaponToDropIfAny = m_currentWeapon;
	}

	try
	{
		// 重置冷卻時間
		m_pickUpWeaponTimeCounter = m_pickUpWeaponCooldown;

		// 處理舊武器（如果需要）
		if (weaponToDropIfAny)
		{
			// 重新激活舊武器的 InteractableComponent
			if (auto interactableComp =
					weaponToDropIfAny->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
			{
				interactableComp->SetComponentActive(true);
			}
			RemoveWeapon(weaponToDropIfAny); // 移除舊武器
		}
		else if (m_currentWeapon)
		{
			m_currentWeapon->SetControlVisible(false);
		}

		// 添加新武器
		m_Weapons.push_back(newWeapon);
		m_currentWeapon = newWeapon;
		m_currentWeapon->SetOwner(character);

		if (auto followerComp = m_currentWeapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
		{
			followerComp->SetFollower(character);
		}
		m_currentWeapon->SetControlVisible(true);

		if (!m_currentWeapon->IsRegisteredToScene())
		{
			scene->GetPendingObjects().emplace_back(m_currentWeapon);
			m_currentWeapon->SetRegisteredToScene(true);
		}

		interactableManager->QueueUnregister(newWeapon);

		if (weaponToDropIfAny)
		{
			interactableManager->RegisterInteractable(weaponToDropIfAny);
		}

		// 播放拾取武器音效
		AudioManager::GetInstance().PlaySFX("pick_up_weapon");

		// LOG_DEBUG("Successfully picked up weapon: {}", newWeapon->GetName());
		return true;
	}
	catch (...)
	{
		LOG_ERROR("Failed to pick up weapon: {}", newWeapon->GetName());
		// 如果出現異常，不進行任何註冊操作，保持原狀
		return false;
	}
}

void AttackComponent::AddWeapon(const std::shared_ptr<Weapon> &newWeapon)
{
	auto character = GetOwner<Character>();
	if (!character)
		return;
	if (m_Weapons.size() > m_maxWeapon)
	{
		LOG_ERROR("over maxWeapon");
		RemoveWeapon(m_currentWeapon); // 移除舊武器
	}
	if (m_currentWeapon)
		m_currentWeapon->SetControlVisible(false);

	m_Weapons.push_back(newWeapon); // 添加新武器列表
	m_currentWeapon = newWeapon; // 更新當前武器
	m_currentWeapon->SetOwner(character); // 當前武器添加擁有者的指標
	if (auto followerComp = m_currentWeapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
	{
		followerComp->SetFollower(character);
	}
	m_currentWeapon->SetControlVisible(true);

	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	scene->GetPendingObjects().emplace_back(m_currentWeapon);
	m_currentWeapon->SetRegisteredToScene(true);
}

void AttackComponent::RemoveWeapon(const std::shared_ptr<Weapon> &weapon)
{
	m_Weapons.erase(std::remove(m_Weapons.begin(), m_Weapons.end(), weapon), m_Weapons.end());
	weapon->m_Transform.rotation = 0;
	auto scale = weapon->m_Transform.scale;
	weapon->m_Transform.scale = glm::vec2(std::abs(scale.x), std::abs(scale.y));
	if (auto followerComp = weapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
		followerComp->SetFollower(nullptr);
}

void AttackComponent::switchWeapon()
{
	if (m_switchTimeCounter > 0)
		return;
	if (m_Weapons.empty())
		return;
	// 重置冷卻時間
	m_switchTimeCounter = m_switchCooldown;

	// 播放武器切換音效
	AudioManager::GetInstance().PlaySFX("switch");

	// OpenGL 本身是「狀態機 + 立即模式」，它不會自己管理任何物件，它只會畫你給它的東西
	m_currentWeapon->SetControlVisible(false);

	// 找目前武器的位置
	auto it = std::find(m_Weapons.begin(), m_Weapons.end(), m_currentWeapon);
	if (it != m_Weapons.end() && std::next(it) != m_Weapons.end())
	{
		m_currentWeapon = *std::next(it);
	}
	else
	{
		m_currentWeapon = m_Weapons.front(); // 循環回到第一把武器
	}
	m_currentWeapon->SetControlVisible(true);
}

DamageResult AttackComponent::calculateDamage()
{
	// 計算總暴擊率
	float totalCritRate = m_criticalRate + m_currentWeapon->GetCriticalRate();

	// 限制暴擊率在0-100%之間
	totalCritRate = std::clamp(totalCritRate, 0.0f, 1.0f);

	// 是否觸發暴擊
	bool isCrit = (rand() / static_cast<float>(RAND_MAX)) < totalCritRate;

	// 傷害計算公式
	int baseDamage = m_currentWeapon->GetDamage();
	int finalDamage = isCrit ? baseDamage * (1.5) // 暴擊傷害==>150%
							 : baseDamage; // 普通傷害

	return {finalDamage, isCrit};
}

void AttackComponent::TryAttack()
{
	auto character = GetOwner<Character>();
	if (!character)
		return;

	const auto healthComponent = character->GetComponent<HealthComponent>(ComponentType::HEALTH);
	if (!healthComponent)
		return;
	if (const auto stateComp = character->GetComponent<StateComponent>(ComponentType::STATE))
	{
		if (auto statusEffects = stateComp->GetActiveEffects(); !statusEffects.empty())
		{
			const auto it = std::find_if(statusEffects.begin(), statusEffects.end(),
										 [](StatusEffect effect) { return effect == StatusEffect::FROZEN; });
			// 如果有Frozen異常狀態，就不能攻擊
			if (it != statusEffects.end())
				return;
		}
	}
	const bool isPlayer = (character->GetType() == CharacterType::PLAYER);
	const float currentEnergy = healthComponent->GetCurrentEnergy();
	const auto useEnergy = m_currentWeapon->GetEnergy();

	if (isPlayer && currentEnergy <= 0 && useEnergy != 0)
	{
		// LOG_DEBUG("AttackComponent: Not enough energy to attack");
		return;
	}

	// 主武器攻击
	if (m_currentWeapon && m_currentWeapon->CanAttack())
	{
		if (currentEnergy >= m_currentWeapon->GetEnergy())
		{
			auto damageResult = calculateDamage();
			m_currentWeapon->attack(damageResult.damage, damageResult.isCriticalHit);

			if (isPlayer)
			{
				healthComponent->ConsumeEnergy(useEnergy);
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
				const auto damageResult = calculateDamage();
				m_secondWeapon->attack(damageResult.damage, damageResult.isCriticalHit);

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
		// LOG_DEBUG("second weapon is nullptr");
		return;
	}
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!enable)
	{
		scene->GetRoot().lock()->RemoveChild(m_secondWeapon);
		// scene->GetCamera().lock()->RemoveChild(m_secondWeapon);
		scene->GetCamera().lock()->MarkForRemoval(m_secondWeapon);
		m_secondWeapon = nullptr;
	}
	else
	{
		scene->GetRoot().lock()->AddChild(m_secondWeapon);
		scene->GetCamera().lock()->SafeAddChild(m_secondWeapon);
	}
}

void AttackComponent::OnTargetPositionUpdate(std::weak_ptr<Character> enemy)
{
	if (auto locked = enemy.lock())
	{
		this->SetTarget(std::dynamic_pointer_cast<nGameObject>(locked));
		// 通知目前武器最靠近的目標
		if (const auto followerComp = m_currentWeapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
		{
			followerComp->SetTarget(std::dynamic_pointer_cast<nGameObject>(locked));
		}
		// 若有雙武器也通知
		if (m_dualWield)
		{
			if (const auto followerComp2 = m_secondWeapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
			{
				followerComp2->SetTarget(std::dynamic_pointer_cast<nGameObject>(locked));
			}
		}
	}
}

void AttackComponent::OnLostTarget()
{
	m_Target.reset();
	for (const auto &weapon : m_Weapons)
	{
		if (const auto followerComp = weapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
		{
			followerComp->SetTarget(nullptr);
		}
	}

	if (m_dualWield)
	{
		if (const auto followerComp2 = m_secondWeapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
		{
			followerComp2->SetTarget(nullptr);
		}
	}
}

std::vector<EventType> AttackComponent::SubscribedEventTypes() const
{
	return {EventType::Death, EventType::ShowUp, EventType::Hide};
}

void AttackComponent::HandleEvent(const EventInfo &eventInfo)
{
	// {}可以在case裏形成額外作用域，用來在裏面定義變數
	switch (eventInfo.GetEventType())
	{
	case EventType::Death:
		{
			if (const auto *deathEventInfo = dynamic_cast<const DeathEventInfo *>(&eventInfo))
			{
				// 清除武器
				// m_Weapons.clear();
				for (const auto &weapon : m_Weapons)
				{
					weapon->SetControlVisible(false);
				}
			}
			break;
		}
	case EventType::ShowUp:
		{
			if (const auto *showUpEvent = dynamic_cast<const ShowUpEvent *>(&eventInfo))
			{
				if (m_currentWeapon)
					m_currentWeapon->SetControlVisible(true);

				// 如果有雙持武器也顯示
				if (m_dualWield && m_secondWeapon)
				{
					m_secondWeapon->SetControlVisible(true);
				}
			}
			break;
		}
	case EventType::Hide:
		{
			if (const auto *hideEvent = dynamic_cast<const HideEvent *>(&eventInfo))
			{
				if (m_currentWeapon)
					m_currentWeapon->SetControlVisible(false);

				// 如果有雙持武器也隱藏
				if (m_dualWield && m_secondWeapon)
				{
					m_secondWeapon->SetControlVisible(false);
				}
			}
			break;
		}
	default:
		break;
	}
}

void AttackComponent::ChangeCurrentWeapon(const int id)
{
    auto character = GetOwner<Character>();
    if (!character)
    {
        LOG_ERROR("ChangeCurrentWeapon: Character is null");
        return;
    }

    const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
    if (!scene)
    {
        LOG_ERROR("ChangeCurrentWeapon: Scene is null");
        return;
    }

    auto room = scene->GetCurrentRoom();
    if (!room)
    {
        LOG_ERROR("ChangeCurrentWeapon: Room is null");
        return;
    }

    auto interactableManager = room->GetInteractionManager();
    if (!interactableManager)
    {
        LOG_ERROR("ChangeCurrentWeapon: InteractableManager is null");
        return;
    }
	
    auto newWeapon = WeaponFactory::createWeapon(id);
    if (!newWeapon)
    {
        LOG_ERROR("ChangeCurrentWeapon: Failed to create weapon with ID {}", id);
        return;
    }

    try
    {
        // 處理舊武器（如果存在）
        if (m_currentWeapon)
        {
            interactableManager->QueueUnregister(m_currentWeapon);
            scene->GetRoot().lock()->RemoveChild(m_currentWeapon);
            scene->GetCamera().lock()->SafeRemoveChild(m_currentWeapon);
            RemoveWeapon(m_currentWeapon);
            m_currentWeapon = nullptr; // 確保舊武器指針清空
        }

        // 添加新武器
        m_Weapons.push_back(newWeapon);
        m_currentWeapon = newWeapon;
        m_currentWeapon->SetOwner(character);

        // 設置 FollowerComponent
        if (auto followerComp = m_currentWeapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
        {
            followerComp->SetFollower(character);
        }
        m_currentWeapon->SetControlVisible(true);

        // 將新武器添加到場景
        if (!m_currentWeapon->IsRegisteredToScene())
        {
            scene->GetRoot().lock()->AddChild(m_currentWeapon);
            scene->GetCamera().lock()->SafeAddChild(m_currentWeapon);
            m_currentWeapon->SetRegisteredToScene(true);
        }

        // 新武器不應作為可交互物件
        interactableManager->QueueUnregister(newWeapon);

        // 播放替換武器音效
        AudioManager::GetInstance().PlaySFX("pick_up_weapon");

        // LOG_DEBUG("Successfully changed to weapon ID: {}, Name: {}", id, newWeapon->GetName());
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("ChangeCurrentWeapon: Exception occurred: {}", e.what());
    }
}