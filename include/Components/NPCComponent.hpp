//
// Created by tjx20 on 6/13/2025.
//

#ifndef NPCCOMPONENT_HPP
#define NPCCOMPONENT_HPP

#include "Components/Component.hpp"
#include "Structs/NPCComponentStruct.hpp"
#include "Creature/Character.hpp"
#include "Components/InteractableComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Components/AnimationComponent.hpp"
#include "Animation.hpp"
#include <string>
#include <vector>
#include <functional>

class nGameObject;
class InteractableComponent;
class StateComponent;
class AnimationComponent;

class NPCComponent final : public Component
{
public:
	NPCComponent(const StructNPCComponent& config);

	void Init() override;
	void Update() override;

	void SetDialogue(const std::vector<std::string>& lines);
	void StartDialogue(const std::shared_ptr<Character>& interactor);
	void ResetDialogueState();
	void ShowNextDialogue();
	void SetActionCallback(const std::function<void(const std::shared_ptr<Character>&)>& callback);
	bool IsActionReady() const { return m_isActionReady; }

private:
	void CreatePromptObject();
	void UpdatePromptText();
	void ExecuteAction(const std::shared_ptr<Character>& player);

	std::vector<std::string> m_dialogueLines;
	size_t m_currentLineIndex = 0;
	bool m_isInDialogue = false;
	bool m_isActionReady = false;
	std::shared_ptr<nGameObject> m_promptObject;
	std::string m_promptText;
	float m_promptSize;
	std::function<void(const std::shared_ptr<Character>&)> m_actionCallback;
};

#endif // NPCCOMPONENT_HPP
