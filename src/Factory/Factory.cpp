//
// Created by tjx20 on 3/14/2025.
//

#include "Factory/Factory.hpp"

#include "Util/Logger.hpp"

ZIndexType Factory::stringToZIndexType(const std::string& zIndexStr) {
	if (zIndexStr == "FLOOR") return FLOOR;
	if (zIndexStr == "OBJECTLOW") return OBJECTLOW;
	if (zIndexStr == "BULLET") return BULLET;
	if (zIndexStr == "OBJECTHIGH") return OBJECTHIGH;
	if (zIndexStr == "UI") return UI;
	if (zIndexStr == "CUSTOM") return CUSTOM;
	LOG_ERROR("Unknown zIndex: {}", zIndexStr);
	return CUSTOM;
}

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

void Factory::createComponent(const std::shared_ptr<nGameObject>& object, const nlohmann::json &json)
{
	static const std::unordered_map<std::string, std::function<void()>> componentBluePrint = {
		{"COLLISION",
			[object,json]() {
				object->AddComponent<CollisionComponent>
				(ComponentType::COLLISION,json.get<StructComponents::StructCollisionComponent>());
			}
		},
	};
	const std::string& componentClass = json.at("Class").get<std::string>();

	if (const auto component = componentBluePrint.find(componentClass); component != componentBluePrint.end()) {
		component->second();
	}
	LOG_DEBUG("ErrorInFactory: Wrong Class: {}",componentClass);
}
