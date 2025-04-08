//
// Created by QuzzS on 2025/4/8.
//
#include "Components/InteractableComponent.hpp"
#include "Util/Text.hpp"

void InteractableComponent::Init() {
	CreatePrompt();
}

void InteractableComponent::Update() {
	// 檢查自動互動邏輯
	if (m_IsAutoInteract && m_IsPlayerNearby) {
		if (const auto player = m_Player.lock()) {
			OnInteract(player);
		}
	}

	// 更新提示位置
	if (m_PromptObject && m_IsPromptVisible) {
		auto owner = GetOwner<nGameObject>();
		if (owner) {
			// 設置提示在互動物體上方
			m_PromptObject->SetWorldCoord(owner->GetWorldCoord() + glm::vec2(0.0f,owner->GetImageSize().y) );
		}
	}
}

bool InteractableComponent::OnInteract(const std::shared_ptr<Character>& interactor) {
	if (m_InteractionCallback) {
		m_InteractionCallback(interactor, GetOwner<nGameObject>());
		return true;
	}
	return false;
}

void InteractableComponent::ShowPrompt(bool show) {
	if (m_PromptObject) {
		m_PromptObject->SetVisible(show);
		m_IsPromptVisible = show;
	}
}

bool InteractableComponent::IsInRange(const std::shared_ptr<Character>& character) const {
	if (!character) return false;

	auto owner = GetOwner<nGameObject>();
	if (!owner) return false;

	float distance = glm::length(character->GetWorldCoord() - owner->GetWorldCoord());
	return distance <= m_InteractionRadius;
}

void InteractableComponent::CreatePrompt() {
	m_PromptObject = std::make_shared<nGameObject>("InteractionPrompt");
	// 這裡可以設置提示的外觀，例如文字或圖示
	m_PromptObject->SetDrawable(std::make_shared<Util::Text>(RESOURCE_DIR"/Font/zpix.ttf",20,m_PromptText,Util::Color(255,255,255)));
	m_PromptObject->SetZIndexType(ZIndexType::UI);
	m_PromptObject->SetZIndex(1.0f);
	m_PromptObject->SetVisible(false);

	// 通過場景添加到渲染樹 (需要在實現時處理)
}