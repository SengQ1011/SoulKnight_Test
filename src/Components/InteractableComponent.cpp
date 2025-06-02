//
// Created by QuzzS on 2025/4/8.
//

#include "Components/InteractableComponent.hpp"

#include "Components/ChestComponent.hpp"
#include "Scene/SceneManager.hpp"

#include "Components/walletComponent.hpp"
#include "Creature/Character.hpp"
#include "Scene/Dungeon_Scene.hpp"


void InteractableComponent::Init()
{
	// TODO:可能要換位置
	if ( m_Type == InteractableType::PORTAL)
	{
		m_InteractionCallback =
			[](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
		{
			const auto &scene = SceneManager::GetInstance().GetCurrentScene().lock();
			scene->SetIsChange(true);
		};
	}
	else if (m_Type == InteractableType::REWARD_CHEST || m_Type == InteractableType::WEAPON_CHEST)
	{
		m_InteractionCallback =
			[](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
			{ if (auto chestComp = target->GetComponent<ChestComponent>(ComponentType::CHEST))
				{
					chestComp->ChestOpened();
				}
			};
	}
	else if (m_Type == InteractableType::COIN || m_Type == InteractableType::ENERGY_BALL)
	{
		// 設定自動追蹤玩家的行為
		m_UpdateCallback = [](const std::shared_ptr<nGameObject> &self, const std::shared_ptr<Character> &player)
		{
			glm::vec2 selfPos = self->GetWorldCoord();
			glm::vec2 playerPos = player->GetWorldCoord();
			float distance = glm::distance(selfPos, playerPos);

			const float attractRange = 100.0f;
			if (distance < attractRange)
			{
				glm::vec2 direction = glm::normalize(playerPos - selfPos);
				const float speed = 200.0f; // px/sec
				self->SetWorldCoord(direction * speed * (Util::Time::GetDeltaTimeMs() / 1000.0f));
			}
		};

		// 設定碰到玩家時的效果
		m_InteractionCallback = [](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
		{
			if (const auto walletComp = interactor->GetComponent<WalletComponent>(ComponentType::WALLET))
			{
				walletComp->AddMoney(2);
				const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
				if (!currentScene) return;
				currentScene->GetRoot().lock()->RemoveChild(interactor);
				currentScene->GetCamera().lock()->SafeRemoveChild(target);
			}
		};
	}

}

void InteractableComponent::Update()
{
	//更新位置
	if (!m_PromptObject)
		return;
	if (const auto owner = GetOwner<nGameObject>())
	{
		m_PromptObject->SetWorldCoord(owner->GetWorldCoord() + glm::vec2(10.0f, owner->GetImageSize().y + std::sin(timer) * 10.0f));
		timer += Util::Time::GetDeltaTimeMs() / 1000.0f;
		if (timer >= 3.1415) timer = 0.0f;
	}
	if (m_UpdateCallback)
	{
		auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
		if (!scene) return;

		std::shared_ptr<Character> player = nullptr;
		if (std::shared_ptr<DungeonScene> currentScene = std::dynamic_pointer_cast<DungeonScene>(scene))
		{
			player = currentScene->GetPlayer();
		}
		const auto self = GetOwner<nGameObject>();

		if (player && self)
			m_UpdateCallback(self, player);
	}
}

bool InteractableComponent::OnInteract(const std::shared_ptr<Character> &interactor)
{
	if (m_InteractionCallback)
	{
		m_InteractionCallback(interactor, GetOwner<nGameObject>());
		return true;
	}
	return false;
}

void InteractableComponent::ShowPrompt(bool show)
{
	if (m_PromptObject)
	{
		m_PromptObject->SetControlVisible(show);
		m_IsPromptVisible = show;
	}
}

bool InteractableComponent::IsInRange(const std::shared_ptr<Character> &character) const
{
	if (!character)
		return false;

	auto owner = GetOwner<nGameObject>();
	if (!owner)
		return false;

	float distance = glm::length(character->GetWorldCoord() - owner->GetWorldCoord());
	return distance <= m_InteractionRadius;
}

