//
// Created by tjx20 on 3/14/2025.
//

#include "Factory/Factory.hpp"

#include "Util/Logger.hpp"

Factory::Factory() {}


nlohmann::json Factory::readJsonFile(const std::string& fileName) {
	std::ifstream file(JSON_DIR "/" + fileName);
	if (!file.is_open()) {
		LOG_DEBUG("Error: Unable to open file: {}",fileName);
		return nlohmann::json();  // 如果文件打開失敗，返回空的 JSON 物件
	}

	nlohmann::json jsonData;
	file >> jsonData;
	return jsonData;
}

std::shared_ptr<Component> Factory::createComponent(const nlohmann::json &json)
{
	static const std::unordered_map<std::string, std::function<std::shared_ptr<Component>(const nlohmann::json&)>> componentBluePrint = {
		{"CollisionComponent",
			[](const nlohmann::json &data) {
				return std::make_shared<CollisionComponent>(data.get<StructComponents::StructCollisionComponent>());
			}
		},
	};
	const std::string& componentClass = json.at("Class").get<std::string>();

	if (const auto component = componentBluePrint.find(componentClass); component != componentBluePrint.end()) {
		return component->second(json);
	}
	LOG_DEBUG("ErrorInFactory: Wrong Class: {}",componentClass);
	return nullptr;
}
