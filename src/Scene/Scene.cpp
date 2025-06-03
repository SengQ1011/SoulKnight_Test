//
// Created by QuzzS on 2025/4/25.
//

#include "Scene/Scene.hpp"
#include "Components/WalletComponent.hpp"
#include "Override/nGameObject.hpp"
#include "SaveManager.hpp"
#include "Scene/SceneManager.hpp"

void Scene::Upload()
{
	auto &sceneManager = SceneManager::GetInstance();
	sceneManager.UploadGameProgress(m_SceneData);
};

void Scene::Download()
{
	const auto &sceneManager = SceneManager::GetInstance();
	m_SceneData = sceneManager.DownloadGameProgress();
};

void Scene::FlushPendingObjectsToRendererAndCamera()
{
	for (const std::shared_ptr<nGameObject> &obj : m_PendingObjects)
	{
		if (!obj)
			continue;

		const auto renderer = m_Root;
		const auto camera = m_Camera;

		if (renderer && obj->GetDrawable())
			renderer->AddChild(obj);
		if (camera)
			camera->AddChild(obj);
	}
	m_PendingObjects.clear();
}

void Scene::SavePlayerInformation(std::shared_ptr<Character> player) const
{
	// 檢查 m_SceneData 是否為空（測試模式或直接進入場景時可能為空）
	if (!m_SceneData)
	{
		LOG_DEBUG("m_SceneData is nullptr, skipping save player information (likely in test mode)");
		return;
	}

	if (player)
	{
		int hp, energy, money = 0;
		std::vector<int> weaponID, talentID;
		if (auto healthComp = player->GetComponent<HealthComponent>(ComponentType::HEALTH))
		{
			hp = healthComp->GetCurrentHp();
			energy = healthComp->GetCurrentEnergy();
		}
		if (auto walletComp = player->GetComponent<WalletComponent>(ComponentType::WALLET))
		{
			money = walletComp->GetMoney();
		}
		if (auto attackComp = player->GetComponent<AttackComponent>(ComponentType::ATTACK))
		{
			weaponID = attackComp->GetAllWeaponID();
		}
		if (auto talentComp = player->GetComponent<TalentComponent>(ComponentType::TALENT))
		{
			talentID = talentComp->GetAllTalentID();
		}

		m_SceneData->gameProgress.currentStage++;
		// playerData
		m_SceneData->gameProgress.playerData.currentHp = hp;
		m_SceneData->gameProgress.playerData.currentEnergy = energy;
		m_SceneData->gameProgress.playerData.money = money;
		m_SceneData->gameProgress.playerData.weaponID = weaponID;
		m_SceneData->gameProgress.playerData.talentID = talentID;
	}
}
