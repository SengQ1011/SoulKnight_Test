#include "Components/NPCComponent.hpp"
#include "Components/InteractableComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Creature/Character.hpp"
#include "Scene/Dungeon_Scene.hpp"
#include "Scene/Lobby_Scene.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Logger.hpp"
#include "ImagePoolManager.hpp"

NPCComponent::NPCComponent(const StructNPCComponent& config)
    : Component(ComponentType::NPC)
    , m_dialogueLines(config.dialogues)
    , m_isInDialogue(false)
    , m_isActionReady(false)
    , m_promptText(config.promptText)
    , m_promptSize(config.promptSize) {}

void NPCComponent::Init() {
    CreatePromptObject();
}

void NPCComponent::CreatePromptObject() {
    if (!m_promptObject) {
        m_promptObject = std::make_shared<nGameObject>("NPCPrompt");
        auto text = ImagePoolManager::GetInstance().GetText(
            RESOURCE_DIR "/Font/jf-openhuninn-2.1.ttf", m_promptSize,
            m_promptText, Util::Color(255, 255, 255), false);

        m_promptObject->SetDrawable(text);
        m_promptObject->SetZIndexType(ZIndexType::UI);
        m_promptObject->SetZIndex(10.0f);
        m_promptObject->SetControlVisible(false);
        m_promptObject->SetInitialScale(glm::vec2(0.5f));
        m_promptObject->SetInitialScaleSet(true);

        if (const auto scene = SceneManager::GetInstance().GetCurrentScene().lock()) {
            scene->GetPendingObjects().emplace_back(m_promptObject);
            m_promptObject->SetRegisteredToScene(true);
            scene->FlushPendingObjectsToRendererAndCamera();
        }
    }
}

void NPCComponent::UpdatePromptText() {
    if (auto interactableComp = GetOwner<nGameObject>()->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE)) {
        if (m_isActionReady) {
            interactableComp->SetPromptText("按F確認");
        }
        else if (m_isInDialogue && m_currentLineIndex < m_dialogueLines.size()) {
            interactableComp->SetPromptText(m_dialogueLines[m_currentLineIndex]);
        } else {
            interactableComp->SetPromptText(m_promptText);
        }
    }
}

void NPCComponent::SetDialogue(const std::vector<std::string>& lines) {
    m_dialogueLines = lines;
    m_currentLineIndex = 0;
    m_isInDialogue = false;
    m_isActionReady = false;
    UpdatePromptText();
}

void NPCComponent::StartDialogue(const std::shared_ptr<Character>& interactor) {
    if (!interactor) return;

    // 如果準備執行行動
    if (m_isActionReady) {
        ExecuteAction(interactor);
        return;
    }

    // 如果不在對話中，開始新對話
    if (!m_isInDialogue) {
        m_isInDialogue = true;
        m_currentLineIndex = 0;
        
        // 更新動畫狀態
        if (auto stateComp = GetOwner<nGameObject>()->GetComponent<StateComponent>(ComponentType::STATE)) {
            stateComp->SetState(State::SKILL);
        }
        
        // 顯示第一句對話
        UpdatePromptText();
    }
    // 如果正在對話中，顯示下一句
    else {
        ShowNextDialogue();
    }
}

void NPCComponent::ShowNextDialogue() {
    m_currentLineIndex++;
    
    // 如果還有更多對話
    if (m_currentLineIndex < m_dialogueLines.size()) {
        UpdatePromptText();
    }
    // 如果對話結束
    else {
        m_isInDialogue = false;
        m_isActionReady = true;
        
        // 重置動畫狀態
        if (auto stateComp = GetOwner<nGameObject>()->GetComponent<StateComponent>(ComponentType::STATE)) {
            stateComp->SetState(State::STANDING);
        }
        
        // 更新提示文字
        UpdatePromptText();
    }
}

void NPCComponent::ExecuteAction(const std::shared_ptr<Character>& player) {
    if (m_actionCallback) {
        m_actionCallback(player);
    }
    ResetDialogueState();
}

void NPCComponent::SetActionCallback(const std::function<void(const std::shared_ptr<Character>&)>& callback) {
    m_actionCallback = callback;
}

void NPCComponent::Update() {
    // 检查玩家是否离开互动范围
    if (m_isInDialogue || m_isActionReady) {
        if (auto interactableComp = GetOwner<nGameObject>()->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE)) {
            if (auto scene = SceneManager::GetInstance().GetCurrentScene().lock()) {
                std::shared_ptr<Character> player = nullptr;

                if (auto dungeon = std::dynamic_pointer_cast<DungeonScene>(scene)) {
                    player = dungeon->GetPlayer();
                }
                else if (auto lobby = std::dynamic_pointer_cast<LobbyScene>(scene)) {
                    player = lobby->GetPlayer();
                }

                if (player && !interactableComp->IsInRange(player)) {
                    ResetDialogueState();
                }
            }
        }
    }

    // 检查动画是否播放完成
    if (auto animationComp = GetOwner<nGameObject>()->GetComponent<AnimationComponent>(ComponentType::ANIMATION)) {
        if (animationComp->IsUsingSkillEffect()) {
            if (auto animation = std::dynamic_pointer_cast<Animation>(animationComp->GetAnimation(State::SKILL))) {
                if (animation->IfAnimationEnds()) {
                    // 动画播放完成，停止效果
                    animationComp->SetSkillEffect(false);
                }
            }
        }
    }
}

void NPCComponent::ResetDialogueState() {
    m_isInDialogue = false;
    m_isActionReady = false;
    m_currentLineIndex = 0;
    UpdatePromptText();
}
