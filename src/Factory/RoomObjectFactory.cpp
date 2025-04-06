//
// Created by QuzzS on 2025/3/27.
//

#include "Factory/RoomObjectFactory.hpp"

#include "Components/AnimationComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/FollowerComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/StateComponent.hpp"

std::shared_ptr<RoomObject> RoomObjectFactory::createRoomObject(const std::string& _id, const std::string& _class)
{
	std::shared_ptr<RoomObject> roomObject = std::make_shared<RoomObject>();
	// 讀取 JSON 資料
	nlohmann::json origin = readJsonFile("LobbyObjectData.json");

	auto data = origin[_id];

	//設置Drawable
	roomObject->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR+data.at("path").get<std::string>()));

	//設置ZIndexLayer
	if (data.contains("ZIndex"))
	{
		const auto zIndexStr = data.at("ZIndex").get<std::string>();
		roomObject->SetZIndexType(stringToZIndexType(zIndexStr));
		// if (zIndex_str == "ObjectLow") roomObject->SetZIndexType(ZIndexType::OBJECTLOW);
		// else if (zIndex_str == "Bullet") roomObject->SetZIndexType(ZIndexType::BULLET);
		// else if (zIndex_str == "ObjectHigh") roomObject->SetZIndexType(ZIndexType::OBJECTHIGH);
		// else if (zIndex_str == "UI") roomObject->SetZIndexType(ZIndexType::UI);
		// else if (zIndex_str == "Floor") roomObject->SetZIndexType(ZIndexType::FLOOR);
		// else LOG_DEBUG("RoomObjectFactory::createRoomObject: Unknown ZIndexType");
	}
	else LOG_DEBUG("RoomObjectFactory::createRoomObject: No such ZIndex!");

	//設置Components
	if (!data.contains("components")) {return roomObject;}
	for (auto& component : data.at("components"))
	{
		if (component.at("Name").get<std::string>() != "collision_box") continue;
		// auto newComponent = createComponent(component);
		// LOG_DEBUG("success2 {}", newComponent->GetType() == ComponentType::COLLISION);
		roomObject->AddComponent<CollisionComponent>(ComponentType::COLLISION,component.get<StructComponents::StructCollisionComponent>());
	}
	return roomObject;
}

std::vector<std::shared_ptr<RoomObject>> RoomObjectFactory::createObjectsFromJson(const nlohmann::json& objectsJson) {
	std::vector<std::shared_ptr<RoomObject>> objects;
	for (const auto& objData : objectsJson) {
		objects.push_back(createRoomObject(objData["id"], objData["class"]));
	}
	return objects;
}