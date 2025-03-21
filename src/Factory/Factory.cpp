//
// Created by tjx20 on 3/14/2025.
//

#include "Factory/Factory.hpp"

Factory::Factory() {}

nlohmann::json Factory::readJsonFile(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		LOG_DEBUG("Error: Unable to open file: {}",filePath);
		return nlohmann::json();  // 如果文件打開失敗，返回空的 JSON 物件
	}

	nlohmann::json jsonData;
	file >> jsonData;
	return jsonData;
}