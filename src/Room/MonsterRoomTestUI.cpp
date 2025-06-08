//
// Created by Assistant on 2025/1/27.
//

#include "Room/MonsterRoomTestUI.hpp"

#include "Loader.hpp"
#include "Room/MonsterRoom.hpp"
#include "Room/RoomLayoutManager.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"


// ImGui 標頭檔
#include <imgui.h>

MonsterRoomTestUI::MonsterRoomTestUI(std::shared_ptr<MonsterRoom> monsterRoom) : m_MonsterRoom(monsterRoom)
{
	RefreshAvailableLayouts();
}

void MonsterRoomTestUI::RenderUI()
{
	// 檢查快捷鍵開啟/關閉 UI
	if (Util::Input::IsKeyDown(Util::Keycode::F1))
	{
		m_ShowUI = !m_ShowUI;
	}

	if (!m_ShowUI)
		return;

	// 渲染主視窗
	if (ImGui::Begin("Monster Room Layout Tester", &m_ShowUI))
	{
		ImGui::Text("Monster Room Layout Manager");
		ImGui::Separator();

		// 重新整理按鈕
		if (ImGui::Button("Refresh Layouts"))
		{
			RefreshAvailableLayouts();
		}

		ImGui::SameLine();
		ImGui::Text("Total: %zu layouts", m_AvailableLayouts.size());

		// 隨機佈局按鈕
		ImGui::Separator();
		if (ImGui::Button("Random Layout", ImVec2(200.0f, 30.0f)))
		{
			m_RequestedLayout = "RANDOM"; // 特殊標識符表示隨機
			m_HasLayoutChangeRequest = true;
			LOG_INFO("Random layout change requested");
		}

		ImGui::Separator();
		ImGui::Text("Available Layouts:");

		// 渲染佈局按鈕
		RenderLayoutButtons();

		ImGui::Separator();
		ImGui::Text("Controls:");
		ImGui::Text("- Press F1 to toggle this window");
		ImGui::Text("- Click 'Random Layout' for random selection");
		ImGui::Text("- Click specific layout to switch immediately");
	}
	ImGui::End();
}

void MonsterRoomTestUI::ClearLayoutChangeRequest()
{
	m_HasLayoutChangeRequest = false;
	m_RequestedLayout.clear();
}

void MonsterRoomTestUI::RefreshAvailableLayouts()
{
	if (auto room = m_MonsterRoom.lock())
	{
		try
		{
			// 使用 RoomLayoutManager 獲取實際的佈局列表
			RoomLayoutManager layoutManager("IcePlains");
			layoutManager.ScanAvailableLayouts("MonsterRoom");
			m_AvailableLayouts = layoutManager.GetAllLayoutNames("MonsterRoom");

			LOG_DEBUG("Refreshed available layouts: {} found", m_AvailableLayouts.size());

			// 如果沒有找到佈局，提供預設值
			if (m_AvailableLayouts.empty())
			{
				LOG_WARN("No MonsterRoom layouts found, using fallback");
				m_AvailableLayouts = {"2717ObjectPosition_1", "ObjectPosition"};
			}
		}
		catch (const std::exception &e)
		{
			LOG_ERROR("Failed to refresh layouts: {}", e.what());
			// 使用預設佈局作為後備
			m_AvailableLayouts = {"2717ObjectPosition_1", "ObjectPosition"};
		}
	}
}

void MonsterRoomTestUI::RenderLayoutButtons()
{
	if (m_AvailableLayouts.empty())
	{
		ImGui::Text("No layouts available");
		return;
	}

	// 計算按鈕佈局
	float buttonWidth = 180.0f;
	float windowWidth = ImGui::GetContentRegionAvail().x;
	int buttonsPerRow = std::max(1, static_cast<int>(windowWidth / (buttonWidth + ImGui::GetStyle().ItemSpacing.x)));

	for (size_t i = 0; i < m_AvailableLayouts.size(); ++i)
	{
		const std::string &layoutName = m_AvailableLayouts[i];

		// 創建按鈕
		if (ImGui::Button(layoutName.c_str(), ImVec2(buttonWidth, 0)))
		{
			m_RequestedLayout = layoutName;
			m_HasLayoutChangeRequest = true;
			LOG_INFO("Layout change requested: {}", layoutName);
		}

		// 換行邏輯
		if ((i + 1) % buttonsPerRow != 0 && i + 1 < m_AvailableLayouts.size())
		{
			ImGui::SameLine();
		}

		// 顯示佈局資訊（滑鼠懸停時）
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Layout: %s", layoutName.c_str());

			// 解析佈局名稱以顯示尺寸資訊
			if (layoutName.find("1717") != std::string::npos)
			{
				ImGui::Text("Size: 17x17");
			}
			else if (layoutName.find("2717") != std::string::npos)
			{
				ImGui::Text("Size: 27x17");
			}
			else if (layoutName == "ObjectPosition")
			{
				ImGui::Text("Size: 27x27 (Default)");
			}
			else
			{
				ImGui::Text("Size: Unknown");
			}

			ImGui::Text("Click to switch to this layout");
			ImGui::EndTooltip();
		}
	}
}
