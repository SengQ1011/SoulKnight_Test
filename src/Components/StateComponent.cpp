//
// Created by tjx20 on 3/24/2025.
//

#include "Components/StateComponent.hpp"

#include "Animation.hpp"
#include "Components/AnimationComponent.hpp"
#include "Creature/Character.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Time.hpp"

StateComponent::StateComponent() {}

void StateComponent::Init()
{
	m_currentState = State::STANDING;
	// 預先建立所有可能狀態 icon
	auto& imagePool = ImagePoolManager::GetInstance();
	for (const auto& [effect, visual] : effectVisualMap) {
		std::shared_ptr<Core::Drawable> icon;
		if (effect == StatusEffect::FROZEN)
			icon = imagePool.GetImage(visual.fullBodyEffectPath);
		else
			icon = imagePool.GetImage(visual.iconPath);
		m_allIcons[effect] = icon;
	}
	// 初始化
	const auto owner = GetOwner<Character>();
	if (!owner) return;
	m_icon1 = std::make_shared<nGameObject>();
	m_icon2 = std::make_shared<nGameObject>();
	m_icon1->SetZIndexType(ZIndexType::UI);
	m_icon2->SetZIndexType(ZIndexType::UI);
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	scene->GetRoot().lock()->AddChild(m_icon1);
	scene->GetCamera().lock()->AddChild(m_icon1);
	scene->GetRoot().lock()->AddChild(m_icon2);
	scene->GetCamera().lock()->AddChild(m_icon2);
}

void StateComponent::Update() {
	const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

	if(!m_statusEffectTimers.empty())
	{
		std::vector<StatusEffect> expiredEffects;
		for (auto& [effect, timer] : m_statusEffectTimers) {
			timer -= deltaTime;
			// 處理各種狀態效果的持續效果
			ProcessStatusEffectTick(effect, deltaTime);
			if (timer <= 0.0f)
				expiredEffects.push_back(effect);
		}
		// 统一移除
		for (const auto& effect : expiredEffects) {
			RemoveStatusEffect(effect);
		}
	}

	//======= 更新状态图示 =====
	UpdateIconPosition();
}

void StateComponent::ProcessStatusEffectTick(StatusEffect effect, float deltaTime) {
	auto owner = GetOwner<Character>();
	if (!owner) return;
	const auto healthComp = owner->GetComponent<HealthComponent>(ComponentType::HEALTH);
	if (!healthComp) { LOG_ERROR("HealthComponent is null"); return; }

	switch (effect) {
	case StatusEffect::BURNS:
	{
		// 每秒造成1点火元素伤害
		static float burnDamageTimer = 0.0f;
		burnDamageTimer += deltaTime;
		if (burnDamageTimer >= 1.0f) {
			// 造成火元素伤害
			healthComp->TakeDamage(1);
			burnDamageTimer = 0.0f;
		}
	}
		break;

	case StatusEffect::POISON:
	{
		// 每2秒造成1点毒元素伤害
		static float poisonDamageTimer = 0.0f;
		poisonDamageTimer += deltaTime;
		if (poisonDamageTimer >= 2.0f) {
			// 造成毒元素伤害
			healthComp->TakeDamage(1);
			poisonDamageTimer = 0.0f;
		}
	}
		break;

	case StatusEffect::FROZEN:
		SetState(State::STANDING);
		break;

	default:
		break;
	}
}

void StateComponent::UpdateIconPosition() {
	const auto owner = GetOwner<nGameObject>();
	if (!owner) return;

	if (m_statusEffects.empty()) {
		if (m_icon1) m_icon1->SetControlVisible(false);
		if (m_icon2) m_icon2->SetControlVisible(false);
	}
	else {
		// 更新图标位置，但需要区分是否为全身效果
		auto updateIconPosition = [&](std::shared_ptr<nGameObject> icon, const glm::vec2& offset) {
			if (!icon || !icon->GetDrawable()) return;

			// 检查这个图标对应的状态效果类型
			StatusEffect iconEffect = StatusEffect::NONE; // 默认值
			for (const auto& [effect, drawable] : m_allIcons) {
				if (drawable == icon->GetDrawable()) {
					iconEffect = effect;
					break;
				}
			}

			// 如果是全身效果（如FROZEN），不应用偏移
			if (effectVisualMap[iconEffect].isFullBodyEffect) {
				icon->m_WorldCoord = owner->GetWorldCoord();
			} else {
				icon->m_WorldCoord = owner->GetWorldCoord() + offset;
			}
		};

		if (m_statusEffects.size() == 1) {
			updateIconPosition(m_icon1, glm::vec2(0.0f, 16.0f));
			updateIconPosition(m_icon2, glm::vec2(0.0f, 16.0f));
		}
		else if (m_statusEffects.size() == 2) {
			updateIconPosition(m_icon1, glm::vec2(-12.0f, 16.0f));
			updateIconPosition(m_icon2, glm::vec2(12.0f, 16.0f));
		}
	}
}

void StateComponent::SetState(State newState)
{
	if (newState != m_currentState)
	{
		auto character = GetOwner<Character>();
		if (character)
		{
			auto animationComponent = character->GetComponent<AnimationComponent>(ComponentType::ANIMATION);
			auto m_currentAnimation = animationComponent->GetCurrentAnimation();
			if (character->GetCharacterName() == "Knight" && newState == State::SKILL)
			{
				animationComponent->SetSkillEffect(true);
			}
			else
			{
				m_currentState = newState;
				// LOG_DEBUG("Changing State to {}",std::to_string(static_cast<int>(newState)));

				// 根据当前状态切换动画
				if (m_currentAnimation)
				{
					m_currentAnimation->PlayAnimation(false);
					animationComponent->SetAnimation(m_currentState);
				}
			}
		}
	}
}

void StateComponent::ApplyStatusEffect(const StatusEffect& effect)
{
	const auto owner = GetOwner<Character>();
	const auto healthComp = owner->GetComponent<HealthComponent>(ComponentType::HEALTH);
	if (!healthComp) { LOG_ERROR("HealthComponent is null"); return; }
	const auto movementComp = owner->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
	if (!movementComp) { LOG_ERROR("MovementComponent is null"); return; }


	float duration = 0.0f;
	switch (effect) {
	case StatusEffect::BURNS:
		duration = 2.0f;
		break;
	case StatusEffect::POISON:
		duration = 5.0f;
		// 移速降低30%
		movementComp->SetCurrentSpeedRatio(movementComp->GetSpeedRatio() * 0.7f);
		break;
	case StatusEffect::FROZEN:
		duration = 2.5f;
		// 免疫伤害，无法行动
		healthComp->SetInvincibleMode(true);
		movementComp->SetCurrentSpeedRatio(0.0f);
		SetState(State::STANDING);
		break;

	default:
		return;
	}

	// 如果已經有該狀態，就更新 timer（延長/重置時間）
	if (m_statusEffectTimers.count(effect)) {
		m_statusEffectTimers[effect] = duration;
	} else
	{
		// 沒有該狀態：加進去 + 可加進 vector 作為記錄
		m_statusEffectTimers[effect] = duration;
		m_statusEffects.push_back(effect);

		if (m_icon1->GetDrawable() == nullptr)
		{
			m_icon1->SetDrawable(m_allIcons[effect]);
			m_icon1->SetControlVisible(true);
		}else if(m_icon2->GetDrawable() == nullptr)
		{
			m_icon2->SetDrawable(m_allIcons[effect]);
			m_icon2->SetControlVisible(true);
		}
		else LOG_ERROR("StateComponent::don't have empty m_icon");
	}
}

void StateComponent::RemoveStatusEffect(const StatusEffect& effect) {
	// 从计时器和效果列表中移除
	m_statusEffectTimers.erase(effect);
	// 先std::remove 重排把要移除的元素放到最后，iterator到end()后再真正删除
	m_statusEffects.erase(std::remove(m_statusEffects.begin(), m_statusEffects.end(), effect), m_statusEffects.end());
	if (m_icon1 && m_allIcons[effect] == m_icon1->GetDrawable()) {
		m_icon1->SetDrawable(nullptr);
		m_icon1->SetControlVisible(false);
	} else if (m_icon2 && m_allIcons[effect] == m_icon2->GetDrawable()) {
		m_icon2->SetDrawable(nullptr);
		m_icon2->SetControlVisible(false);
	}

	auto owner = GetOwner<Character>();
	if (!owner) return;
	const auto healthComp = owner->GetComponent<HealthComponent>(ComponentType::HEALTH);
	if (!healthComp) { LOG_ERROR("HealthComponent is null"); return; }
	const auto movementComp = owner->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
	if (!movementComp) { LOG_ERROR("MovementComponent is null"); return; }

	// 移除状态效果的影响
	if (owner) {
		switch (effect) {
		case StatusEffect::POISON:
			// 恢复移速
			movementComp->SetCurrentSpeedRatio(movementComp->GetSpeedRatio());
			break;
		case StatusEffect::FROZEN:
			// 恢复行动能力
			movementComp->SetCurrentSpeedRatio(movementComp->GetSpeedRatio());
			healthComp->SetInvincibleMode(false);
			// owner->PauseSkillCooldowns(false);
			break;
		default:
			break;
		}
	}
}

void StateComponent::HideAllIcon() const
{
	m_icon1->SetControlVisible(false);
	m_icon2->SetControlVisible(false);
}
