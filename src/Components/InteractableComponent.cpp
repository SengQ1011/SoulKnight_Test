//
// Created by QuzzS on 2025/4/8.
//
#include "Scene/SceneManager.hpp"
#include "Components/InteractableComponent.hpp"
#include "Util/Text.hpp"

void InteractableComponent::Init() {
	//TODO:可能要換位置
	if (m_InteractableFunctionName == "Portal")
	{
		m_InteractionCallback = [](
			const std::shared_ptr<Character>& interactor,
			const std::shared_ptr<nGameObject>& target)
		{
			const auto& scene = SceneManager::GetInstance().GetCurrentScene().lock();
			scene->SetIsChange(true);
		};
	}
}

void InteractableComponent::Update() {
	//更新位置
	if (!m_PromptObject) return;
	if (const auto owner = GetOwner<nGameObject>())
	{
		m_PromptObject->SetWorldCoord(owner->GetWorldCoord() + glm::vec2(10.0f,owner->GetImageSize().y) );
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
