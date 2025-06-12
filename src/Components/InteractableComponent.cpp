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
#include "Shop/ShopTable.hpp"
#include "Weapon/Weapon.hpp"


void InteractableComponent::Init()
{
	switch (m_interactableType)
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
				case Scene::SceneType::DungeonLoad:
					// 地牢的傳送門進入下一關或結算
					// 只有在地牢中的傳送門才會增加關卡數
					{
						// 先檢查當前關卡數來決定是否已經在Boss關
						auto saveData = SaveManager::GetInstance().GetSaveData();
						bool isBossStage = (saveData && saveData->gameProgress.currentStage == 5);

						// 嘗試將當前場景轉換為 DungeonScene
						if (auto dungeonScene = std::dynamic_pointer_cast<DungeonScene>(scene))
						{
							dungeonScene->OnStageCompleted();
						}

						// 如果是Boss關（第5關）完成，直接進入結算場景
						if (isBossStage)
						{
							// 完成第5關（Boss關）後進入結算場景
							targetScene = Scene::SceneType::Result;
						}
						else
						{
							// 其他情況進入下一關
							targetScene = Scene::SceneType::DungeonLoad;
						}
					}
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
				LOG_DEBUG("InteractableComponent::InteractableType::REWARD_CHEST");
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
					walletComp->AddMoney(1);
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
					healthComp->AddCurrentEnergy(8);
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
	case InteractableType::WEAPON:
		{
			m_InteractionCallback =
				[this](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
			{
				if (const auto attackComp = interactor->GetComponent<AttackComponent>(ComponentType::ATTACK))
				{
					if (const auto weapon = std::dynamic_pointer_cast<Weapon>(target))
					{
						// 🔥 關鍵修復：只有拾取成功時才停用 InteractableComponent
						bool pickupSuccess = attackComp->PickUpWeapon(weapon);

						if (pickupSuccess)
						{
							ShowPrompt(false);
							// 武器被拾取後停用 InteractableComponent
							SetComponentActive(false);
						}
						// 如果拾取失敗（例如冷卻期間），保持互動組件活躍
					}
				}
			};


			// 為武器創建文字提示，參考寶箱的設置
			if (!m_PromptObject)
			{
				auto weaponPrompt = std::make_shared<nGameObject>("WeaponPrompt");
				std::shared_ptr<Core::Drawable> promptText;
				if (const auto weapon = GetOwner<Weapon>())
				{
					promptText =
						ImagePoolManager::GetInstance().GetText(RESOURCE_DIR "/Font/jf-openhuninn-2.1.ttf", 24.0f,
																weapon->GetName(), Util::Color(255, 255, 255), false);
				}
				else
				{
					promptText = ImagePoolManager::GetInstance().GetText(
						RESOURCE_DIR "/Font/jf-openhuninn-2.1.ttf", 24.0f, "武器", Util::Color(255, 255, 255), false);
				}
				weaponPrompt->SetDrawable(promptText);
				weaponPrompt->SetZIndexType(ZIndexType::UI);
				weaponPrompt->SetZIndex(10.0f);
				weaponPrompt->SetControlVisible(false);
				weaponPrompt->SetInitialScale(glm::vec2(0.5f));
				weaponPrompt->SetInitialScaleSet(true);

				const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
				if (currentScene)
				{
					currentScene->GetPendingObjects().emplace_back(weaponPrompt);
					weaponPrompt->SetRegisteredToScene(true);
				}

				m_PromptObject = weaponPrompt;
			}
			break;
		}
	case InteractableType::SHOP_TABLE:
		{
			m_InteractionCallback =
				[this](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
			{
				// 將 target 轉換為 ShopTable
				if (auto shopTable = std::dynamic_pointer_cast<ShopTable>(target))
				{
					shopTable->PurchaseItem(interactor);
				}
			};

			// 為商品桌子創建價格提示
			if (!m_PromptObject)
			{
				auto pricePrompt = std::make_shared<nGameObject>("ShopTablePrompt");

				// 從 ShopTable 取得價格資訊
				if (auto shopTable = GetOwner<ShopTable>())
				{
					std::string priceText = std::to_string(shopTable->GetPrice()) + " 金幣";
					auto promptText = std::make_shared<Util::Text>(RESOURCE_DIR "/Font/jf-openhuninn-2.1.ttf", 24.0f,
																   priceText, Util::Color(255, 255, 0), false);
					pricePrompt->SetDrawable(promptText);
				}
				// else
				// {
				// 	auto promptText = ImagePoolManager::GetInstance().GetText(
				// 		RESOURCE_DIR "/Font/jf-openhuninn-2.1.ttf", 24.0f, "商品", Util::Color(255, 255, 255), false);
				// 	pricePrompt->SetDrawable(promptText);
				// }

				pricePrompt->SetZIndexType(ZIndexType::UI);
				pricePrompt->SetZIndex(10.0f);
				pricePrompt->SetControlVisible(false);
				pricePrompt->SetInitialScale(glm::vec2(0.5f));
				pricePrompt->SetInitialScaleSet(true);

				const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
				if (currentScene)
				{
					currentScene->GetPendingObjects().emplace_back(pricePrompt);
					pricePrompt->SetRegisteredToScene(true);
				}

				m_PromptObject = pricePrompt;
			}
			break;
		}
	case InteractableType::HP_POISON:
		{
			// 從 ShopTable 取得價格資訊
			if (auto item = GetOwner<nGameObject>())
			{
				std::string priceText;
				if (item->GetName().find("small") != std::string::npos)
				{
					priceText = "小回復藥水";
					m_InteractionCallback =
						[](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
					{
						if (const auto healthComp = interactor->GetComponent<HealthComponent>(ComponentType::HEALTH))
						{
							healthComp->AddCurrentHp(1);
							target->SetControlVisible(false);
							target->SetActive(false);
						}
					};
				}
				else
				{
					priceText = "大回復藥水";
					m_InteractionCallback =
						[](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
					{
						if (const auto healthComp = interactor->GetComponent<HealthComponent>(ComponentType::HEALTH))
						{
							healthComp->AddCurrentHp(4);
							target->SetControlVisible(false);
							target->SetActive(false);
						}
					};
				}

				// 為商品桌子創建價格提示
				if (!m_PromptObject)
				{
					auto hpPotionPromp = std::make_shared<nGameObject>("hpPotionPrompt");

					auto promptText = ImagePoolManager::GetInstance().GetText(
						RESOURCE_DIR "/Font/jf-openhuninn-2.1.ttf", 24.0f, priceText, Util::Color(255, 255, 0), false);
					hpPotionPromp->SetDrawable(promptText);

					hpPotionPromp->SetZIndexType(ZIndexType::UI);
					hpPotionPromp->SetZIndex(10.0f);
					hpPotionPromp->SetControlVisible(false);
					hpPotionPromp->SetInitialScale(glm::vec2(0.5f));
					hpPotionPromp->SetInitialScaleSet(true);

					const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
					if (currentScene)
					{
						currentScene->GetPendingObjects().emplace_back(hpPotionPromp);
						hpPotionPromp->SetRegisteredToScene(true);
					}

					m_PromptObject = hpPotionPromp;
				}
			}
			break;
		}
	case InteractableType::ENERGY_POISON:
		{
			// 從 ShopTable 取得價格資訊
			if (auto item = GetOwner<nGameObject>())
			{
				std::string priceText;
				if (item->GetName().find("small") != std::string::npos)
				{
					priceText = "小能量藥水";
					m_InteractionCallback =
						[](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
					{
						if (const auto healthComp = interactor->GetComponent<HealthComponent>(ComponentType::HEALTH))
						{
							healthComp->AddCurrentEnergy(50);
							target->SetControlVisible(false);
							target->SetActive(false);
						}
					};
				}
				else
				{
					priceText = "大能量藥水";
					m_InteractionCallback =
						[](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
					{
						if (const auto healthComp = interactor->GetComponent<HealthComponent>(ComponentType::HEALTH))
						{
							healthComp->AddCurrentEnergy(100);
							target->SetControlVisible(false);
							target->SetActive(false);
						}
					};
				}

				// 為能量藥水創建價格提示
				if (!m_PromptObject)
				{
					auto energyPotionPrompt = std::make_shared<nGameObject>("energyPotionPrompt");

					auto promptText = ImagePoolManager::GetInstance().GetText(
						RESOURCE_DIR "/Font/jf-openhuninn-2.1.ttf", 24.0f, priceText, Util::Color(0, 255, 255), false);
					energyPotionPrompt->SetDrawable(promptText);

					energyPotionPrompt->SetZIndexType(ZIndexType::UI);
					energyPotionPrompt->SetZIndex(10.0f);
					energyPotionPrompt->SetControlVisible(false);
					energyPotionPrompt->SetInitialScale(glm::vec2(0.5f));
					energyPotionPrompt->SetInitialScaleSet(true);

					const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
					if (currentScene)
					{
						currentScene->GetPendingObjects().emplace_back(energyPotionPrompt);
						energyPotionPrompt->SetRegisteredToScene(true);
					}

					m_PromptObject = energyPotionPrompt;
				}
			}
			break;
		}
	default:
		LOG_ERROR("InteractableComponent::Init() miss m_interactableType");
		break;
		;
	}

	if (m_interactableType == InteractableType::COIN || m_interactableType == InteractableType::ENERGY_BALL)
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
				const float speed = 100.0f;
				self->SetWorldCoord(selfPos + (direction * speed * (Util::Time::GetDeltaTimeMs() / 1000.0f)));
			}
		};
	}

	m_PromptUI = std::make_shared<nGameObject>("InteractableUI");
	if (m_PromptUI)
	{
		// 設置UI的基本屬性
		m_PromptUI->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_interactablePrompt.png"));
		m_PromptUI->SetZIndexType(ZIndexType::CUSTOM);
		m_PromptUI->SetZIndex(100.0f);
		m_PromptUI->SetInitialScale(glm::vec2(2.0f));
		m_PromptUI->SetInitialScaleSet(true);
		m_PromptUI->SetControlVisible(false); // 初始隱藏

		const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
		if (currentScene)
		{
			currentScene->GetPendingObjects().emplace_back(m_PromptUI);
			m_PromptUI->SetRegisteredToScene(true);
		}
	}

	// 初始化浮動計時器，設置為無限循環的2π週期
	m_FloatTimer.SetDuration(1.0f); // 2π 秒為一個完整週期
	m_FloatTimer.Start();
}

void InteractableComponent::Update()
{
	// 如果組件非激活狀態，停止更新
	if (!m_IsComponentActive)
		return;

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
	UpdatePromptPositions();
}

bool InteractableComponent::OnInteract(const std::shared_ptr<Character> &interactor)
{
	auto item = GetOwner<nGameObject>();
	if (m_InteractionCallback)
	{
		m_InteractionCallback(interactor, GetOwner<nGameObject>());
		if (m_interactableType == InteractableType::REWARD_CHEST ||
			m_interactableType == InteractableType::WEAPON_CHEST)
		{
			// 立即隱藏提示詞，避免在移除前還會顯示
			ShowPrompt(false);

			// 寶箱被開啟後停用 InteractableComponent
			SetComponentActive(false);

			if (const auto interactableManager =
					SceneManager::GetInstance().GetCurrentScene().lock()->GetCurrentRoom()->GetInteractionManager())
			{
				LOG_DEBUG("InteractableComponent::Remove");
				interactableManager->QueueUnregister(item);
			}
		}
		return true;
	}
	return false;
}

void InteractableComponent::ShowPrompt(bool show)
{
	if (m_PromptObject)
	{
		m_PromptObject->SetControlVisible(show);
	}
	if (m_PromptUI)
	{
		m_PromptUI->SetControlVisible(show);
	}
	m_IsPromptVisible = show;
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

glm::vec2 InteractableComponent::UpdatePromptObjectPosition(const std::shared_ptr<nGameObject> &owner)
{
	glm::vec2 promptObjectOffset = glm::vec2(4.0f, 0.0f); // 好懸 爲什麽文字會自己偏移

	if (m_PromptObject && owner)
	{
		// 計算 PromptObject 在 owner 上方的位置
		promptObjectOffset.y = (owner->GetImageSize().y + m_PromptObject->GetImageSize().y) / 2.0f;
		glm::vec2 promptObjectPosition = owner->GetWorldCoord() + promptObjectOffset;
		m_PromptObject->SetWorldCoord(promptObjectPosition);
	}

	return promptObjectOffset;
}

void InteractableComponent::UpdatePromptPositions()
{
	if (!m_PromptObject)
		return;

	const auto owner = GetOwner<nGameObject>();
	if (!owner)
		return;

	// 更新 PromptObject 位置
	glm::vec2 promptObjectOffset = UpdatePromptObjectPosition(owner);

	// 更新 PromptUI 位置
	if (m_PromptUI)
	{
		UpdatePromptUIPosition(owner, promptObjectOffset);
	}
}

void InteractableComponent::UpdatePromptUIPosition(const std::shared_ptr<nGameObject> &owner,
												   const glm::vec2 &promptObjectOffset)
{
	if (!m_PromptUI || !m_PromptObject || !owner)
		return;

	// 更新浮動計時器
	m_FloatTimer.Update();

	// 如果計時器完成，重新開始（實現無限循環）
	if (m_FloatTimer.IsFinished())
	{
		m_FloatTimer.Restart();
	}

	// 使用 Timer 的進度來計算浮動偏移
	float progress = m_FloatTimer.GetProgress(); // 0.0f 到 1.0f
	float radians = progress * 2.0f * static_cast<float>(M_PI); // 轉換為 0 到 2π 弧度
	float floatOffset = std::sin(radians) * 2.5f; // 浮動範圍2.5像素

	// 計算 PromptUI 位置：在 PromptObject 下方，加上浮動偏移
	glm::vec2 promptUIOffset = glm::vec2(0.0f, promptObjectOffset.y);
	promptUIOffset.y -= (m_PromptObject->GetImageSize().y + m_PromptUI->GetImageSize().y) / 2.0f;
	promptUIOffset.y += floatOffset; // 添加浮動效果

	glm::vec2 promptUIPosition = owner->GetWorldCoord() + promptUIOffset;
	m_PromptUI->SetWorldCoord(promptUIPosition);
}

void InteractableComponent::SetPromptText(const std::string &text)
{
	if (!m_PromptObject)
		return;

	// 嘗試將 Drawable 轉換為 Text 物件
	if (const auto drawable = std::dynamic_pointer_cast<Util::Text>(m_PromptObject->GetDrawable()))
	{
		drawable->SetText(text);
	}
}
