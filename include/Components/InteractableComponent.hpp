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

	// 使用std::function作為互動回調
	using InteractionCallback = std::function<void(
		const std::shared_ptr<Character>& interactor,
		const std::shared_ptr<nGameObject>& target
	)>;

	void Init() override;
	void Update() override;

	// 主要功能：處理互動事件
	virtual bool OnInteract(const std::shared_ptr<Character>& interactor);

	// 設置互動回調
	void SetInteractionCallback(const InteractionCallback& callback) {
		m_InteractionCallback = callback;
	}

	// 檢查玩家是否在互動範圍內
	bool IsInRange(const std::shared_ptr<Character>& character) const;
	// 顯示/隱藏互動提示
	void ShowPrompt(bool show);

	// Getter
	std::string GetPromptText() const { return m_PromptText; }
	float GetInteractionRadius() const { return m_InteractionRadius; }
	bool IsPlayerNearby() const { return m_IsPlayerNearby; }
	bool IsAutoInteract() const { return m_IsAutoInteract; }

	//Setter
	void SetPromptText(const std::string& text) { m_PromptText = text; }
	void SetInteractionRadius(float radius) { m_InteractionRadius = radius; }
	void SetPlayerNearby(bool nearby) { m_IsPlayerNearby = nearby; }
	void SetAutoInteract(bool autoInteract) { m_IsAutoInteract = autoInteract; }

protected:
	std::string m_PromptText;
	float m_InteractionRadius;
	bool m_IsAutoInteract;
	bool m_IsPlayerNearby = false;
	bool m_IsPromptVisible = false;

	std::weak_ptr<Character> m_Player; // 記錄正在互動的玩家
	InteractionCallback m_InteractionCallback;

	// 互動提示UI元素
	std::shared_ptr<nGameObject> m_PromptObject = nullptr;

private:
	// 創建互動提示
	void CreatePrompt();
};

#endif //INTERACTABLECOMPONENT_HPP
