//
// Created by tjx20 on 5/26/2025.
//

#include "SaveManager.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

SaveManager::SaveManager(const std::string &saveDir)
	: saveDirectory(saveDir), saveSlot(std::make_shared<SaveData>())
{
	// 確保存檔目錄存在
	CreateSaveDirectory();
	LoadSaveData();
}

bool SaveManager::HasSaveData() const {
	try {
		const std::string filename = saveDirectory + "save_game.json";
		std::ifstream file(filename);
		if (!file.is_open()) return false;

		// 檢查檔案內容是否為空
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string content = buffer.str();

		if (content.empty()) return false;

		nlohmann::json j = nlohmann::json::parse(content);
		if (j.is_null() || j.empty()) return false;

		return true;
	}
	catch (const std::exception& e) {
		LOG_INFO("Error checking save_game.json file: {}", e.what());
		return false;
	}
}

bool SaveManager::SaveGame(const std::shared_ptr<SaveData>& saveData) {
	try {
		saveSlot = saveData;
		saveData->saveTime = GetCurrentTimeString(); // 自動設置保存時間

		const std::string filename = saveDirectory + "save_game.json";
		std::ofstream file(filename);
		if (!file.is_open()) return false;

		nlohmann::json j = saveSlot->toJson();
		// pretty format(自動縮排（indentation）空格數)
		file << j.dump(4);
		file.close();

		LOG_INFO("Game saved successfully");
		return true;
	}
	catch (const std::exception& e) {
		LOG_ERROR("Exception occurred while writing to file: {}", e.what());
		return false;
	}
}

bool SaveManager::DeleteSave() {
	try {
		std::string filename = saveDirectory + "save_game.json";

		if (std::remove(filename.c_str()) == 0) {
			LOG_INFO("Delete successfully");
			// 清除内存中的数据(重置为空)
			saveSlot = nullptr;
			return true;
		}
		return false;
	}
	catch (const std::exception& e) {
		LOG_ERROR("Exception occurred while deleting save:{}", e.what());
		return false;
	}
}

void SaveManager::LoadSaveData() {
	try {
		const std::string filename = saveDirectory + "save_game.json";
		std::ifstream file(filename);
		if (file.is_open()) {
			nlohmann::json j;
			file >> j;
			file.close();

			saveSlot = std::make_shared<SaveData>();
			saveSlot->fromJson(j);
		} else {
			LOG_INFO("No save_game.json file found, creating new save data");
			saveSlot = std::make_shared<SaveData>(); // 給一個新的空白存檔
			SaveGame(saveSlot);
		}
	}
	catch (const std::exception& e) {
		LOG_ERROR("Error loading save_game.json: {}", e.what());
		saveSlot = std::make_shared<SaveData>(); // 保底保證不為 null
	}
}

void SaveManager::CreateSaveDirectory() const {
	// 判斷和建立資料夾
	if (!std::filesystem::exists(saveDirectory)) {
		try {
			std::filesystem::create_directories(saveDirectory);
		} catch (const std::exception& e) {
			LOG_ERROR("Failed to create save directory: {}", e.what());
		}
	}
}

std::string SaveManager::GetCurrentTimeString() {
	const auto now = std::chrono::system_clock::now();
	const std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);

	std::tm tm{};
	localtime_s(&tm, &time_t_now); // 安全版本

	char buffer[100];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
	return std::string(buffer);
}