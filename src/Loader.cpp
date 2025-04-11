//
// Created by QuzzS on 2025/4/11.
//

#include "Loader.hpp"
#include "Util/Logger.hpp"
#include <fstream>

nlohmann::ordered_json Loader::readJsonFile(const std::string &filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open()) {
		LOG_DEBUG("Error: Unable to open file: {}",filePath);
		return nlohmann::json();  // 如果文件打開失敗，返回空的 JSON 物件
	}

	nlohmann::json jsonData;
	file >> jsonData;
	return jsonData;
}

