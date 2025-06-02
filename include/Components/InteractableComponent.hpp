//
// Created by QuzzS on 2025/4/8.
//

#ifndef INTERACTABLECOMPONENT_HPP
#define INTERACTABLECOMPONENT_HPP

#include "Components/Component.hpp"
#include "Structs/InteractionComponentStruct.hpp"

class Character;
class nGameObject;

class InteractableComponent : public Component
{
public:
	explicit InteractableComponent(
		InteractableType type,
		const std::shared_ptr<nGameObject>& promptObject,
		float interactionRadius = 30.0f,
		bool isAutoInteract = false
	) :
		Component(ComponentType::INTERACTABLE),
		m_Type(type),
		m_InteractionRadius(interactionRadius),
		m_IsAutoInteract(isAutoInteract),
		m_PromptObject(promptObject)
	{
	}

	explicit InteractableComponent(const StructInteractableComponent& data) :
		Component(ComponentType::INTERACTABLE),
		m_Type(data.s_InteractableType),
		m_InteractionRadius(data.s_InteractionRadius),
		m_IsAutoInteract(data.s_IsAutoInteract),
		m_PromptObject(data.s_PromptObject)
	{
	}

	// 使用std::function作為互動回調
	using InteractionCallback = std::function<void(const std::shared_ptr<Character> &interactor,
												   const std::shared_ptr<nGameObject> &target)>;
	using UpdateCallback = std::function<void(const std::shared_ptr<nGameObject> &self,
												const std::shared_ptr<Character> &interactor)>;

	void Init() override;
	void Update() override;

	// 主要功能：處理互動事件
	virtual bool OnInteract(const std::shared_ptr<Character> &interactor);

	// 設置互動回調
	void SetInteractionCallback(const InteractionCallback &callback) { m_InteractionCallback = callback; }

	// 檢查玩家是否在互動範圍內
	bool IsInRange(const std::shared_ptr<Character> &character) const;
	// 顯示/隱藏互動提示
	void ShowPrompt(bool show);

	// Getter
	float GetInteractionRadius() const { return m_InteractionRadius; }
	bool IsAutoInteract() const { return m_IsAutoInteract; }
	std::shared_ptr<nGameObject> GetPromptObject() const { return m_PromptObject; }

	// Setter
	void SetInteractionRadius(float radius) { m_InteractionRadius = radius; }
	void SetAutoInteract(bool autoInteract) { m_IsAutoInteract = autoInteract; }

protected:
	InteractableType m_Type;
	float m_InteractionRadius;
	bool m_IsAutoInteract = false;
	bool m_IsPromptVisible = false;

	InteractionCallback m_InteractionCallback;
	UpdateCallback m_UpdateCallback;

	// 互動提示UI元素
	std::shared_ptr<nGameObject> m_PromptObject = nullptr;
	float timer = 0.0f;

	std::array<Uint8, 3> m_Color = {0, 0, 0};
	Uint8 m_PromptColor = 0;
};


#endif //INTERACTABLECOMPONENT_HPP
