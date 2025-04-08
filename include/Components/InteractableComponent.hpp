//
// Created by QuzzS on 2025/4/8.
//

#ifndef INTERACTABLECOMPONENT_HPP
#define INTERACTABLECOMPONENT_HPP

#include "Component.hpp"
#include "Creature/Character.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Text.hpp"
#include "json.hpp"

namespace StructComponents {
	struct StructInteractableComponent
	{
		ComponentType s_Type = ComponentType::INTERACTABLE;
		std::string s_Class = "InteractableComponent";
		float s_InteractionRadius = 30.0f; //71.0f/2
		bool s_IsAutoInteract = false;
		std::shared_ptr<nGameObject> s_PromptObject = nullptr;

		std::string s_InteractableFunctionName = "Portal";
	};

	struct StructPrompt
	{
		//prompt
		std::string s_PromptFontPath = "/Font/zpix.ttf";
		float s_PromptFontSize = 10.0f;
		std::string s_PromptText = "按 F 互動";
		std::array<Uint8,4> s_PromptColor = {255, 255, 255, 255};
		bool s_IsPromptVisible = false;
	};

	inline std::shared_ptr<nGameObject> CreatePromptObject(const StructPrompt& object)
	{
		auto objectPtr = std::make_shared<nGameObject>("InteractionPrompt");
		objectPtr->SetDrawable(std::make_shared<Util::Text>(
			RESOURCE_DIR+object.s_PromptFontPath,
			object.s_PromptFontSize,
			object.s_PromptText,
			Util::Color(object.s_PromptColor[0],object.s_PromptColor[1],object.s_PromptColor[2], object.s_PromptColor[3])));
		objectPtr->SetZIndexType(ZIndexType::UI);
		objectPtr->SetZIndex(10.0f);
		objectPtr->SetVisible(object.s_IsPromptVisible);
		return objectPtr;
	};

	inline void from_json(const nlohmann::json& j, StructPrompt &object)
	{
		if (j.contains("PromptFontPath")) j.at("PromptFontPath").get_to(object.s_PromptFontPath);
		if (j.contains("PromptFontSize")) j.at("PromptFontSize").get_to(object.s_PromptFontSize);
		if (j.contains("PromptText")) j.at("PromptText").get_to(object.s_PromptText);
		if (j.contains("PromptColor")) j.at("PromptColor").get_to(object.s_PromptColor);
		if (j.contains("IsPromptVisible")) j.at("IsPromptVisible").get_to(object.s_IsPromptVisible);
	}

	inline void from_json(const nlohmann::ordered_json &j, StructInteractableComponent &object)
	{
		if (j.contains("Class"))
			j.at("Class").get_to(object.s_Class);
		if (j.contains("InteractionRadius"))
			j.at("InteractionRadius").get_to(object.s_InteractionRadius);
		if (j.contains("IsAutoInteract"))
			j.at("IsAutoInteract").get_to(object.s_IsAutoInteract);
		if (j.contains("InteractableFunctionName"))
			j.at("InteractableFunctionName").get_to(object.s_InteractableFunctionName);
		if (j.contains("PromptObject"))
			object.s_PromptObject = CreatePromptObject(j.get<StructPrompt>());
	}
}

class InteractableComponent : public Component {
public:
	explicit InteractableComponent(
		float interactionRadius = 30.0f,
		bool isAutoInteract = false
	) : Component(ComponentType::INTERACTABLE),
		m_InteractionRadius(interactionRadius),
		m_IsAutoInteract(isAutoInteract) {}

	explicit InteractableComponent(const StructComponents::StructInteractableComponent& data)
		: Component(ComponentType::INTERACTABLE),
		  m_InteractionRadius(data.s_InteractionRadius),
		  m_IsAutoInteract(data.s_IsAutoInteract)
	{
		m_PromptObject = data.s_PromptObject;
		if (auto owner = GetOwner<nGameObject>()) m_PromptObject->SetWorldCoord(owner->GetWorldCoord() + glm::vec2(10.0f,owner->GetImageSize().y) );
		//TODO : 可選function
		if (data.s_InteractableFunctionName == "Portal")
		{
			//TODO: 要換地方
			m_InteractionCallback = [](
				const std::shared_ptr<Character>& interactor,
				const std::shared_ptr<nGameObject>& target)
			{
				auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
				scene->SetIsChange(true);
			};
		}

	};

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
	float GetInteractionRadius() const { return m_InteractionRadius; }
	bool IsPlayerNearby() const { return m_IsPlayerNearby; }
	bool IsAutoInteract() const { return m_IsAutoInteract; }
	std::shared_ptr<nGameObject> GetPromptObject() const { return m_PromptObject; }

	//Setter
	void SetInteractionRadius(float radius) { m_InteractionRadius = radius; }
	void SetPlayerNearby(bool nearby) { m_IsPlayerNearby = nearby; }
	void SetAutoInteract(bool autoInteract) { m_IsAutoInteract = autoInteract; }

protected:
	float m_InteractionRadius; //71.0f/2
	bool m_IsAutoInteract = false;
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
