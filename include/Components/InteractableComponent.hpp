//
// Created by QuzzS on 2025/4/8.
//

#ifndef INTERACTABLECOMPONENT_HPP
#define INTERACTABLECOMPONENT_HPP

#include "Components/Component.hpp"
#include "Structs/InteractionComponentStruct.hpp"
#include "Util/Timer.hpp"

class Character;
class nGameObject;
class Animation;

class InteractableComponent : public Component
{
public:
	explicit InteractableComponent(InteractableType type, const std::shared_ptr<nGameObject> &promptObject,
								   float interactionRadius = 30.0f, bool isAutoInteract = false) :
		Component(ComponentType::INTERACTABLE), m_interactableType(type), m_InteractionRadius(interactionRadius),
		m_IsAutoInteract(isAutoInteract), m_PromptObject(promptObject)
	{
	}

	explicit InteractableComponent(const StructInteractableComponent &data) :
		Component(ComponentType::INTERACTABLE), m_interactableType(data.s_InteractableType),
		m_InteractionRadius(data.s_InteractionRadius), m_IsAutoInteract(data.s_IsAutoInteract),
		m_PromptObject(data.s_PromptObject)
	{
	}

	~InteractableComponent() override = default;

	// 使用std::function作為互動回調
	using InteractionCallback =
		std::function<void(const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)>;
	using UpdateCallback =
		std::function<void(const std::shared_ptr<nGameObject> &self, const std::shared_ptr<Character> &interactor)>;

	void Init() override;
	void Update() override;
	void HandleEvent(const EventInfo &eventInfo) override;
	std::vector<EventType> SubscribedEventTypes() const override;

	// 主要功能：處理互動事件
	virtual bool OnInteract(const std::shared_ptr<Character> &interactor);

	// 設置互動回調
	void SetInteractionCallback(const InteractionCallback &callback) { m_InteractionCallback = callback; }

	// 檢查玩家是否在互動範圍內
	bool IsInRange(const std::shared_ptr<Character> &character) const;
	// 顯示/隱藏互動提示
	void ShowPrompt(bool show);

	// Getter
	[[nodiscard]] float GetInteractionRadius() const { return m_InteractionRadius; }
	[[nodiscard]] bool IsAutoInteract() const { return m_IsAutoInteract; }
	[[nodiscard]] std::shared_ptr<nGameObject> GetPromptObject() const { return m_PromptObject; }
	[[nodiscard]] std::shared_ptr<nGameObject> GetPromptUI() const { return m_PromptUI; }
	[[nodiscard]] bool IsComponentActive() const { return m_IsComponentActive; }

	// Setter
	void SetInteractionRadius(const float radius) { m_InteractionRadius = radius; }
	void SetAutoInteract(const bool autoInteract) { m_IsAutoInteract = autoInteract; }
	void SetPromptUI(const std::shared_ptr<nGameObject> &promptUI) { m_PromptUI = promptUI; }
	void SetPromptObject(const std::shared_ptr<nGameObject> &promptObject) { m_PromptObject = promptObject; }
	void SetInteractableType(const InteractableType type) { m_interactableType = type; }
	void SetComponentActive(const bool active)
	{
		m_IsComponentActive = active;
		if (!active)
		{
			// 立即隱藏所有提示UI
			ShowPrompt(false);

			// 當停用組件時，將UI移到屏幕外並確保隱藏，避免意外顯示
			if (m_PromptUI)
			{
				m_PromptUI->SetWorldCoord(glm::vec2(-10000.0f, -10000.0f));
				m_PromptUI->SetControlVisible(false);
			}
			if (m_PromptObject)
			{
				m_PromptObject->SetWorldCoord(glm::vec2(-10000.0f, -10000.0f));
				m_PromptObject->SetControlVisible(false);
			}
		}
	}

	// 更新 PromptObject 的文本內容
	void SetPromptText(const std::string &text);

	void SetPromptAnimation(const std::shared_ptr<Animation>& animation);

protected:
	InteractableType m_interactableType;
	float m_InteractionRadius;
	bool m_IsAutoInteract = false;
	bool m_IsPromptVisible = false;
	bool m_IsComponentActive = true;

	InteractionCallback m_InteractionCallback;
	UpdateCallback m_UpdateCallback;

	// 互動提示UI元素
	std::shared_ptr<nGameObject> m_PromptObject = nullptr;
	std::shared_ptr<nGameObject> m_PromptUI = nullptr;
	std::shared_ptr<Animation> m_PromptAnimation;
	Util::Timer m_FloatTimer;

	std::array<Uint8, 3> m_Color = {0, 0, 0};
	Uint8 m_PromptColor = 0;

private:
	// 更新提示UI的位置
	void UpdatePromptPositions();
	// 計算並設置 PromptObject 的位置，返回偏移量
	glm::vec2 UpdatePromptObjectPosition(const std::shared_ptr<nGameObject> &owner) const;
	// 計算並設置 PromptUI 的位置
	void UpdatePromptUIPosition(const std::shared_ptr<nGameObject> &owner, const glm::vec2 &promptObjectOffset);
};


#endif // INTERACTABLECOMPONENT_HPP
