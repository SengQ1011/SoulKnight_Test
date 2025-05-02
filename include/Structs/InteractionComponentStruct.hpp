//
// Created by QuzzS on 2025/4/19.
//

#ifndef INTERACTIONCOMPONENTSTRUCT_HPP
#define INTERACTIONCOMPONENTSTRUCT_HPP


#include "json.hpp"
#include "EnumTypes.hpp"

// TODO: refactor
#include "Override/nGameObject.hpp"
#include "Util/Text.hpp"

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
	std::array<Uint8,3> s_PromptColor = {255, 255, 255};
	bool s_IsPromptVisible = false;
};

inline std::shared_ptr<nGameObject> CreatePromptObject(const StructPrompt& object)
{
	auto objectPtr = std::make_shared<nGameObject>("InteractionPrompt");
	auto prompt = std::make_shared<Util::Text>(
		RESOURCE_DIR+object.s_PromptFontPath,
		object.s_PromptFontSize,
		object.s_PromptText,
		Util::Color(object.s_PromptColor[0],object.s_PromptColor[1],object.s_PromptColor[2])
		,false);
	objectPtr->SetDrawable(prompt);
	objectPtr->SetZIndexType(ZIndexType::UI);
	objectPtr->SetZIndex(10.0f);
	objectPtr->SetVisible(object.s_IsPromptVisible);
	objectPtr->SetInitialScale(glm::vec2(0.5f));
	objectPtr->SetInitialScaleSet(true);
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
		object.s_PromptObject = CreatePromptObject(j.at("PromptObject").get<StructPrompt>());
}

#endif //INTERACTIONCOMPONENTSTRUCT_HPP
