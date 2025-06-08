//
// Created by tjx20 on 3/14/2025.
//

#include "Factory/Factory.hpp"

#include "Util/Logger.hpp"
#include "fstream"


#include "Animation.hpp"
#include "Components/ChestComponent.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/DoorComponent.hpp"
#include "Components/InteractableComponent.hpp"

ZIndexType Factory::stringToZIndexType(const std::string &zIndexStr)
{
	if (zIndexStr == "FLOOR")
		return FLOOR;
	if (zIndexStr == "OBJECTLOW")
		return OBJECTLOW;
	if (zIndexStr == "ATTACK")
		return ATTACK;
	if (zIndexStr == "OBJECTHIGH")
		return OBJECTHIGH;
	if (zIndexStr == "UI")
		return UI;
	if (zIndexStr == "CUSTOM")
		return CUSTOM;
	LOG_ERROR("Unknown zIndex: {}", zIndexStr);
	return CUSTOM;
}

ChestType stringToChestType(const std::string &str)
{
	if (str == "WEAPON")
		return ChestType::WEAPON;
	if (str == "REWARD")
		return ChestType::REWARD;
	throw std::invalid_argument("Invalid ChestType: " + str);
}

nlohmann::json Factory::readJsonFile(const std::string &fileName)
{
	std::ifstream file(JSON_DIR "/" + fileName);
	if (!file.is_open())
	{
		LOG_DEBUG("Error: Unable to open file: {}", fileName);
		return nlohmann::json(); // 如果文件打開失敗，返回空的 JSON 物件
	}

	nlohmann::json jsonData;
	file >> jsonData;
	return jsonData;
}

void Factory::createComponent(const std::shared_ptr<nGameObject> &object, const nlohmann::json &json)
{
	// 配對創建組件
	static const std::unordered_map<
		std::string, std::function<void(const std::shared_ptr<nGameObject> &object, const nlohmann::json &json)>>
		componentBluePrint = {
			{"COLLISION",
			 [](const std::shared_ptr<nGameObject> &object, const nlohmann::json &json) {
				 object->AddComponent<CollisionComponent>(ComponentType::COLLISION,
														  json.get<StructCollisionComponent>());
			 }},
			{"INTERACTABLE",
			 [](const std::shared_ptr<nGameObject> &object, const nlohmann::json &json) {
				 object->AddComponent<InteractableComponent>(ComponentType::INTERACTABLE,
															 json.get<StructInteractableComponent>());
			 }},
			{"DOOR", [](const std::shared_ptr<nGameObject> &object, const nlohmann::json &json)
			 { object->AddComponent<DoorComponent>(ComponentType::DOOR); }},
			{"CHEST",
			 [](const std::shared_ptr<nGameObject> &object, const nlohmann::json &json)
			 {
				 try
				 {
					 ChestType chestType = stringToChestType(json.at("chestType").get<std::string>());
					 std::vector<std::string> imagePaths = json.at("imagePaths").get<std::vector<std::string>>();
					 object->AddComponent<ChestComponent>(ComponentType::CHEST, chestType, imagePaths);
				 }
				 catch (const std::exception &e)
				 {
					 LOG_ERROR("Factory::createComponent CHEST parsing failed: {}", e.what());
				 }
			 }},
		};
	const std::string &componentClass = json.at("Class").get<std::string>();

	if (const auto component = componentBluePrint.find(componentClass); component != componentBluePrint.end())
	{
		component->second(object, json);
		return;
	}
	LOG_DEBUG("ErrorInFactory: Wrong Class: {}", componentClass);
}

std::shared_ptr<Animation> Factory::parseAnimations(const nlohmann::json &animationsJson, bool needLoop)
{
	std::vector<std::string> frames;
	for (const auto &frame : animationsJson)
	{
		frames.push_back(RESOURCE_DIR + frame.get<std::string>());
	}
	auto animations = std::make_shared<Animation>(frames, needLoop, "Animation", 0);

	return animations;
}
