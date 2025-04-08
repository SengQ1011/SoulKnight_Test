//
// Created by QuzzS on 2025/4/8.
//

#ifndef INTERACTABLECOMPONENT_HPP
#define INTERACTABLECOMPONENT_HPP

#include "Component.hpp"
#include "Creature/Character.hpp"

class InteractableComponent : public Component {
public:
	explicit InteractableComponent(
		const std::string& promptText = "按 F 互動",
		float interactionRadius = 30.0f,
		bool isAutoInteract = false
	) : Component(ComponentType::INTERACTABLE),
		m_PromptText(promptText),
		m_InteractionRadius(interactionRadius),
		m_IsAutoInteract(isAutoInteract) {}

	// 使用std::function作爲互動回調
	using InteractionCallback = std::function<void(
		const std::shared_ptr<Character>& interactor,
		const std::shared_ptr<nGameObject>& target
		)>;

	void Init() override;
	void Update() override;

	// 主要功能：處理互動事件
	virtual void OnInteract(const std::shared_ptr<Character>& interactor) {
		if (m_InteractionCallback) m_InteractionCallback(interactor,GetOwner<nGameObject>());
	};

	// Getter/Setter
	std::string GetPromptText() const { return m_PromptText; }
	void SetPromptText(const std::string& text) { m_PromptText = text; }

	float GetInteractionRadius() const { return m_InteractionRadius; }
	void SetInteractionRadius(float radius) { m_InteractionRadius = radius; }

	bool CanInteract() const {
		const auto player = m_Player.lock();
		if (!player) return false;
		return player->GetWorldCoord().length() <= m_InteractionRadius;
	};

protected:
	std::string m_PromptText;
	float m_InteractionRadius;
	bool m_IsAutoInteract;
	bool m_IsPlayerNearby = false;

	std::weak_ptr<Character> m_Player; //暫時確定互動物是玩家
	InteractionCallback m_InteractionCallback;

	// 可以添加其他互動相關屬性，如冷卻時間、使用次數限制等
};

#endif //INTERACTABLECOMPONENT_HPP
