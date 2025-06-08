//
// Created by Assistant on 2025/1/27.
//

#ifndef MONSTERROOMTESTUI_HPP
#define MONSTERROOMTESTUI_HPP

#include <memory>
#include <string>
#include <vector>


// 前向聲明
class MonsterRoom;
class Loader;

class MonsterRoomTestUI
{
public:
	explicit MonsterRoomTestUI(std::shared_ptr<MonsterRoom> monsterRoom);
	~MonsterRoomTestUI() = default;

	// 渲染 ImGui 介面
	void RenderUI();

	// 檢查是否有佈局變更請求
	bool HasLayoutChangeRequest() const { return m_HasLayoutChangeRequest; }

	// 取得請求的佈局名稱
	std::string GetRequestedLayout() const { return m_RequestedLayout; }

	// 清除變更請求
	void ClearLayoutChangeRequest();

private:
	std::weak_ptr<MonsterRoom> m_MonsterRoom;
	std::vector<std::string> m_AvailableLayouts;

	// UI 狀態
	bool m_ShowUI = false;
	bool m_HasLayoutChangeRequest = false;
	std::string m_RequestedLayout;

	// 輔助方法
	void RefreshAvailableLayouts();
	void RenderLayoutButtons();
};

#endif // MONSTERROOMTESTUI_HPP
