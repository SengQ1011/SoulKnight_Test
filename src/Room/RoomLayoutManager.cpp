//
// Created by Assistant on 2025/1/27.
//

#include "Room/RoomLayoutManager.hpp"
#include <algorithm>
#include <random>
#include "Util/Logger.hpp"


RoomLayoutManager::RoomLayoutManager(const std::string &theme) : m_Theme(theme), m_RandomEngine(std::random_device{}())
{
	// 預設掃描 MonsterRoom
	ScanAvailableLayouts("MonsterRoom");
}

void RoomLayoutManager::ScanAvailableLayouts(const std::string &roomType)
{
	// 清除該房間類型的現有佈局
	m_AvailableLayouts[roomType].clear();

	// 構建房間目錄路徑
	std::string roomPath = BuildRoomPath(roomType);

	try
	{
		if (!std::filesystem::exists(roomPath))
		{
			if (roomType != "MonsterRoom") return;
			LOG_ERROR("Room directory not found: {}", roomPath);
			return;
		}

		// 遍歷目錄中的所有檔案
		for (const auto &entry : std::filesystem::directory_iterator(roomPath))
		{
			if (entry.is_regular_file())
			{
				std::string filename = entry.path().filename().string();

				// 檢查是否為有效的佈局檔案
				if (IsValidLayoutFile(filename))
				{
					std::string layoutName = ExtractLayoutName(filename);
					m_AvailableLayouts[roomType].push_back(layoutName);
					LOG_DEBUG("Found {} layout: {}", roomType, layoutName);
				}
			}
		}

		// 排序以確保一致性
		std::sort(m_AvailableLayouts[roomType].begin(), m_AvailableLayouts[roomType].end());

		LOG_INFO("Scanned {} available {} layouts for theme '{}'", m_AvailableLayouts[roomType].size(), roomType,
				 m_Theme);
	}
	catch (const std::filesystem::filesystem_error &e)
	{
		LOG_ERROR("Error scanning {} directory: {}", roomType, e.what());
	}
}

std::string RoomLayoutManager::GetRandomLayout(const std::string &roomType)
{
	// 確保該房間類型的佈局已載入
	if (m_AvailableLayouts.find(roomType) == m_AvailableLayouts.end() || m_AvailableLayouts[roomType].empty())
	{
		ScanAvailableLayouts(roomType);
	}

	if (m_AvailableLayouts[roomType].empty())
	{
		LOG_ERROR("No available {} layouts found!", roomType);
		// 根據房間類型返回預設值
		if (roomType == "MonsterRoom")
			return "2717ObjectPosition_1";
		else
			return "ObjectPosition";
	}

	std::uniform_int_distribution<size_t> dist(0, m_AvailableLayouts[roomType].size() - 1);
	size_t randomIndex = dist(m_RandomEngine);

	std::string selectedLayout = m_AvailableLayouts[roomType][randomIndex];

	return selectedLayout;
}

std::vector<std::string> RoomLayoutManager::GetAllLayoutNames(const std::string &roomType) const
{
	auto it = m_AvailableLayouts.find(roomType);
	if (it != m_AvailableLayouts.end())
	{
		return it->second;
	}
	return {};
}

std::string RoomLayoutManager::GetSpecificLayout(const std::string &layoutName, const std::string &roomType)
{
	if (HasLayout(layoutName, roomType))
	{
		LOG_DEBUG("Selected specific {} layout: {}", roomType, layoutName);
		return layoutName;
	}

	LOG_WARN("{} layout '{}' not found, falling back to random selection", roomType, layoutName);
	return GetRandomLayout(roomType);
}

bool RoomLayoutManager::HasLayout(const std::string &layoutName, const std::string &roomType) const
{
	auto it = m_AvailableLayouts.find(roomType);
	if (it != m_AvailableLayouts.end())
	{
		const auto &layouts = it->second;
		return std::find(layouts.begin(), layouts.end(), layoutName) != layouts.end();
	}
	return false;
}

size_t RoomLayoutManager::GetLayoutCount(const std::string &roomType) const
{
	auto it = m_AvailableLayouts.find(roomType);
	if (it != m_AvailableLayouts.end())
	{
		return it->second.size();
	}
	return 0;
}

std::string RoomLayoutManager::BuildRoomPath(const std::string &roomType) const
{
	return JSON_DIR "/" + m_Theme + "/" + roomType;
}

std::string RoomLayoutManager::ExtractLayoutName(const std::string &filename) const
{
	// 移除 .json 副檔名
	if (filename.size() > 5 && filename.substr(filename.size() - 5) == ".json")
	{
		return filename.substr(0, filename.size() - 5);
	}
	return filename;
}

bool RoomLayoutManager::IsValidLayoutFile(const std::string &filename) const
{
	// 檢查是否為 JSON 檔案
	if (filename.size() <= 5 || filename.substr(filename.size() - 5) != ".json")
	{
		return false;
	}

	// 檢查是否包含 "ObjectPosition"
	if (filename.find("ObjectPosition") == std::string::npos)
	{
		return false;
	}

	// 排除不需要的檔案（如果有的話）
	// 可以在這裡添加更多過濾條件

	return true;
}
