//
// Created by QuzzS on 2025/4/25.
//

#include "Room/PortalRoom.hpp"
#include "Components/InteractableComponent.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Logger.hpp"


void PortalRoom::Start(const std::shared_ptr<Character> &player)
{
	DungeonRoom::Start(player);

	CreatePortal();
}


void PortalRoom::Update() { DungeonRoom::Update(); }

void PortalRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadPortalRoomObjectPosition();
	InitializeRoomObjects(jsonData);
}

void PortalRoom::CreatePortal()
{
	const auto portal = m_Factory.lock()->CreateRoomObject("object_transferGate_0", "portal");
	if (!portal) return;

	portal->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);

	// 確保 InteractableComponent 存在
	const auto interactionComp = portal->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE);
	if (!interactionComp) return;

	// 安全設置提示物件位置
	if (const auto promptObj = interactionComp->GetPromptObject())
	{
		// 等待一幀確保動畫大小已初始化
		glm::vec2 portalSize = portal->GetImageSize();
		promptObj->SetWorldCoord(portal->GetWorldCoord());// + glm::vec2(10.0f, portalSize.y / 2));
	}

	AddRoomObject(portal);
}
