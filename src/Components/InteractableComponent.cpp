//
// Created by QuzzS on 2025/4/8.
//

#include "Components/InteractableComponent.hpp"

#include "Components/ChestComponent.hpp"
#include "SaveManager.hpp"
#include "Scene/SceneManager.hpp"

#include "Components/WalletComponent.hpp"
#include "Creature/Character.hpp"
#include "Scene/Dungeon_Scene.hpp"


void InteractableComponent::Init()
{
	switch (m_Type)
	{
	case InteractableType::PORTAL:
		{
			m_InteractionCallback =
				[](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
			{
				// 根據傳送門類型決定要切換到哪個場景
				// 這裡可以根據傳送門的配置或當前場景來決定目標場景

				const auto &scene = SceneManager::GetInstance().GetCurrentScene().lock();
				if (!scene)
					return;

				// 根據當前場景類型決定傳送門的目標
				Scene::SceneType targetScene = Scene::SceneType::Null;

				switch (scene->GetSceneType())
				{
				case Scene::SceneType::Lobby:
					targetScene = Scene::SceneType::DungeonLoad;
					break;
				case Scene::SceneType::Test_KC:
					// 大廳的傳送門通常進入地牢載入場景
					targetScene = Scene::SceneType::DungeonLoad;
					break;
				case Scene::SceneType::Dungeon:
					// 地牢的傳送門進入下一關或結算
					// 只有在地牢中的傳送門才會增加關卡數
					{
						// 嘗試將當前場景轉換為 DungeonScene
						if (auto dungeonScene = std::dynamic_pointer_cast<DungeonScene>(scene))
						{
							dungeonScene->OnStageCompleted();
						}
					}
					targetScene = Scene::SceneType::DungeonLoad;
					break;
				default:
					// 預設行為：使用舊的切換方式
					scene->SetIsChange(true);
					return;
				}

				// 使用新的場景切換方法
				SceneManager::GetInstance().SetNextScene(targetScene);
			};
			break;
		}
	case InteractableType::REWARD_CHEST:
	case InteractableType::WEAPON_CHEST:
		{
			m_InteractionCallback =
				[](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
			{
				if (auto chestComp = target->GetComponent<ChestComponent>(ComponentType::CHEST))
				{
					chestComp->ChestOpened();
				}
			};
			break;
		}
	case InteractableType::COIN:
		{
			m_InteractionCallback =
				[](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
			{
				if (const auto walletComp = interactor->GetComponent<WalletComponent>(ComponentType::WALLET))
				{
					walletComp->AddMoney(2);
					const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
					if (!currentScene)
						return;
					currentScene->GetRoot().lock()->RemoveChild(target);
					currentScene->GetCamera().lock()->SafeRemoveChild(target);
					currentScene->GetCurrentRoom()->GetInteractionManager()->UnregisterInteractable(target);
				}
			};
			break;
		}
	case InteractableType::ENERGY_BALL:
		{
			m_InteractionCallback =
				[](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
			{
				if (const auto healthComp = interactor->GetComponent<HealthComponent>(ComponentType::HEALTH))
				{
					healthComp->AddCurrentEnergy(5);
					const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
					if (!currentScene)
						return;
					currentScene->GetRoot().lock()->RemoveChild(target);
					currentScene->GetCamera().lock()->SafeRemoveChild(target);
					currentScene->GetCurrentRoom()->GetInteractionManager()->UnregisterInteractable(target);
				}
			};
			break;
		}
	}

	if (m_Type == InteractableType::COIN || m_Type == InteractableType::ENERGY_BALL)
	{
		// 設定自動追蹤玩家的行為
		m_UpdateCallback = [](const std::shared_ptr<nGameObject> &self, const std::shared_ptr<Character> &player)
		{
			glm::vec2 selfPos = self->GetWorldCoord();
			glm::vec2 playerPos = player->GetWorldCoord();
			float distance = glm::distance(selfPos, playerPos);

			const float attractRange = 50.0f;
			if (distance < attractRange)
			{
				glm::vec2 direction = glm::normalize(playerPos - selfPos);
				const float speed = 150.0f;
				self->SetWorldCoord(selfPos + (direction * speed * (Util::Time::GetDeltaTimeMs() / 1000.0f)));
			}
		};
	}
}

void InteractableComponent::Update()
{
	if (m_UpdateCallback)
	{
		auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
		if (!scene)
			return;

		std::shared_ptr<Character> player = nullptr;
		if (std::shared_ptr<DungeonScene> currentScene = std::dynamic_pointer_cast<DungeonScene>(scene))
		{
			player = currentScene->GetPlayer();
		}
		const auto self = GetOwner<nGameObject>();

		if (player && self)
			m_UpdateCallback(self, player);
	}
	// 更新位置
	if (!m_PromptObject)
		return;
	if (const auto owner = GetOwner<nGameObject>())
	{
		m_PromptObject->SetWorldCoord(owner->GetWorldCoord() +
									  glm::vec2(10.0f, owner->GetImageSize().y + std::sin(timer) * 10.0f));
		timer += Util::Time::GetDeltaTimeMs() / 1000.0f;
		if (timer >= 3.1415)
			timer = 0.0f;
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
