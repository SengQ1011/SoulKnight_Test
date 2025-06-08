//
// Created by Assistant on 2025/1/27.
//

#ifndef ROOMLAYOUTMANAGER_HPP
#define ROOMLAYOUTMANAGER_HPP

#include <filesystem>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>


class RoomLayoutManager
{
public:
	explicit RoomLayoutManager(const std::string &theme);
	~RoomLayoutManager() = default;

	// 掃描並載入所有可用的房間佈局
	void ScanAvailableLayouts(const std::string &roomType = "MonsterRoom");

	// 隨機選擇一個佈局
	std::string GetRandomLayout(const std::string &roomType = "MonsterRoom");

	// 測試用：取得所有佈局名稱
	std::vector<std::string> GetAllLayoutNames(const std::string &roomType = "MonsterRoom") const;

	// 測試用：指定特定佈局
	std::string GetSpecificLayout(const std::string &layoutName, const std::string &roomType = "MonsterRoom");

	// 檢查佈局是否存在
	bool HasLayout(const std::string &layoutName, const std::string &roomType = "MonsterRoom") const;

	// 取得佈局數量
	size_t GetLayoutCount(const std::string &roomType = "MonsterRoom") const;

private:
	std::string m_Theme;
	std::unordered_map<std::string, std::vector<std::string>> m_AvailableLayouts; // roomType -> layouts
	std::mt19937 m_RandomEngine;

	// 輔助方法
	std::string ExtractLayoutName(const std::string &filename) const;
	bool IsValidLayoutFile(const std::string &filename) const;
	std::string BuildRoomPath(const std::string &roomType) const;
};

#endif // ROOMLAYOUTMANAGER_HPP
