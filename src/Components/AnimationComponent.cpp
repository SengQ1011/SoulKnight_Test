//
// Created by tjx20 on 3/21/2025.
//

#include "Components/AnimationComponent.hpp"

#include "Animation.hpp"
#include "Scene/SceneManager.hpp"

AnimationComponent::AnimationComponent(std::unordered_map<State, std::shared_ptr<Animation>> animations) :
	Component(ComponentType::ANIMATION), m_Animations(std::move(animations))
{
}


void AnimationComponent::Init()
{
	// 初始化動畫
	for (auto &pair : m_Animations)
	{
		if (pair.second)
		{
			pair.second->PlayAnimation(false);
			// LOG_DEBUG("Animation for state " + std::to_string(static_cast<int>(pair.first)) + " initialized");
		}
	}
	SetAnimation(State::STANDING);
	auto character = GetOwner<nGameObject>();
	if (character && m_currentAnimation)
	{
		character->SetDrawable(m_currentAnimation->GetDrawable());
	}
	else
	{
		LOG_ERROR("Failed to add new animation");
	}

	// LOG_DEBUG("Creating Animation Component & successful find owner");
}

void AnimationComponent::Update()
{
	if (m_useSkillEffect && m_effectAnimation) {
		if (auto character = GetOwner<nGameObject>()) {
			m_effectAnimation->m_WorldCoord = character->m_WorldCoord;
			m_effectAnimation->SetZIndexType(ZIndexType::CUSTOM); //可能不該在Update的，但是方便確認
			m_effectAnimation->SetZIndex(character->GetZIndex() - 0.5f);
		}
	}
}

// 支援不同類型的物件
void AnimationComponent::SetAnimation(State state)
{
	auto character = GetOwner<nGameObject>();
	if (character)
	{ // 檢查 owner 是否有效
		auto it = m_Animations.find(state);
		// 有找到相關的動畫
		if (it != m_Animations.end())
		{
			// 移除舊的動畫
			if (m_currentAnimation)
			{
				m_currentAnimation->PlayAnimation(false);
				m_currentAnimation->SetControlVisible(false);
			}

			// 設定新動畫
			m_currentAnimation = it->second;
			m_currentAnimation->SetControlVisible(true);
			m_currentAnimation->PlayAnimation(true);

			// 新動畫設為 Character drawable
			character->SetDrawable(m_currentAnimation->GetDrawable());
			// LOG_DEBUG("Switched animation to state " + std::to_string(static_cast<int>(state)));
		}
	}
	else
	{
		LOG_ERROR("character is no longer valid-->AnimationComponent");
	}

	auto gameObject = GetOwner<nGameObject>();
	if (gameObject)
	{
	}
}

void AnimationComponent::SetSkillEffect(const bool play, const glm::vec2 offset)
{
	if (play) {
		m_useSkillEffect = true;
		auto character = GetOwner<nGameObject>();
		if (character)
		{
			const auto state = State::SKILL;
			auto it = m_Animations.find(state);
			// 有找到相關的動畫
			if (it != m_Animations.end())
			{
				m_effectAnimation = it->second;
				m_effectAnimation->SetControlVisible(true);
				m_effectAnimation->PlayAnimation(true);

				auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
				if (scene) {
				scene->GetRoot().lock()->AddChild(m_effectAnimation);
				scene->GetCamera().lock()->SafeAddChild(m_effectAnimation);
					m_effectAnimation->m_WorldCoord = character->m_WorldCoord + offset;
				}
			}
		}
	}
	else
	{
		m_useSkillEffect = false;
		if (m_effectAnimation) {
		m_effectAnimation->SetControlVisible(false);
		m_effectAnimation->PlayAnimation(false);
		auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
			if (scene) {
		scene->GetRoot().lock()->RemoveChild(m_effectAnimation);
		const auto camera = scene->GetCamera().lock();
				if (camera) {
		camera->MarkForRemoval(m_effectAnimation);
				}
			}
		}
	}
}

