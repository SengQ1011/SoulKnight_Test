//
// Created by tjx20 on 3/21/2025.
//

#include "Components/AnimationComponent.hpp"

#include "Creature/Character.hpp"

AnimationComponent::AnimationComponent(std::unordered_map<State, std::shared_ptr<Animation>> animations)
	:Component(ComponentType::ANIMATION), m_Animations(std::move(animations)){
	LOG_DEBUG("Creating Animation Component & successful find owner");
	// 初始化動畫
	for (auto& pair : m_Animations) {
		if (pair.second) {
			pair.second->PlayAnimation(false);
			LOG_DEBUG("Animation for state " + std::to_string(static_cast<int>(pair.first)) + " initialized");
		}
	}
	SetAnimation(State::STANDING);
	auto character = GetOwner<Character>();
	if (m_currentAnimation) {
		character->AddChild(m_currentAnimation);  // 確保動畫加入角色
		LOG_DEBUG("Successfully added new animation");
	}
	else
	{
		LOG_ERROR("Failed to add new animation");
	}
}


// 支援不同類型的物件
void AnimationComponent::SetAnimation(State state) {
	auto character = GetOwner<Character>();
	if (character) {  // 檢查 owner 是否有效
		auto it = m_Animations.find(state);
		// 有找到相關的動畫
		if (it != m_Animations.end()) {
			// 移除舊的動畫
			if (m_currentAnimation) {
				m_currentAnimation->PlayAnimation(false);
				m_currentAnimation->SetVisible(false);
				character->RemoveChild(m_currentAnimation);  // 使用 owner 訪問 RemoveChild
			}

			// 設定新動畫
			m_currentAnimation = it->second;
			m_currentAnimation->SetVisible(true);
			m_currentAnimation->PlayAnimation(true);

			// 新動畫加入 `Character`
			character->AddChild(m_currentAnimation);  // 使用 owner 訪問 AddChild

			LOG_DEBUG("Switched animation to state " + std::to_string(static_cast<int>(state)));
		}
	} else {
		LOG_ERROR("character is no longer valid-->AnimationComponent");
	}

	auto gameObject = GetOwner<nGameObject>();
	if (gameObject){
	}
}

void AnimationComponent::Update() {
	auto character = GetOwner<Character>();
	if (character) {
		auto stateComponent = character->GetComponent<StateComponent>(ComponentType::STATE);
		auto m_currentState = stateComponent->GetState();
		auto m_prevState = stateComponent->GetPrevState();
		// 根据当前状态切换动画
		if (m_currentState != m_prevState) {
			m_currentAnimation->PlayAnimation(false);
			this->SetAnimation(m_currentState);
			stateComponent->SetPrevState(m_currentState);
		}
	}
}
