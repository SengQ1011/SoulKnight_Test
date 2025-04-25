//
// Created by QuzzS on 2025/4/25.
//

#include "Room/PortalRoom.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"

void PortalRoom::Start(const std::shared_ptr<Character> &player)
{
	Room::Start(player);

	CreateGrid();
	CreatePortal();
}


void PortalRoom::Update()
{
	Room::Update();
}

void PortalRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadPortalRoomObjectPosition();
	InitializeRoomObjects(jsonData);
}

void PortalRoom::CreatePortal()
{
	const auto portal = m_Factory.lock()->createRoomObject("object_transferGate_0","portal");
	portal->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);
	const auto interactionComp = portal->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE);
	interactionComp->GetPromptObject()->SetWorldCoord(portal->GetWorldCoord() + glm::vec2(10.0f,portal->GetImageSize().y/2 - 10.0f));
	RegisterObjectToSceneAndManager(portal);
}

