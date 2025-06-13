//
// Created by QuzzS on 2025/4/20.
//

#include "UIPanel/MinimapPanel.hpp"
#include "ImagePoolManager.hpp"
#include "Override/nGameObject.hpp"
#include "Room/DungeonMap.hpp"
#include "Room/DungeonRoom.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "config.hpp"
#include "imgui.h"


MinimapPanel::MinimapPanel(const std::shared_ptr<DungeonMap> &dungeonMap) : m_DungeonMap(dungeonMap) {}

void MinimapPanel::Start()
{
	// 設置面板名稱
	SetPanelName("minimap");

	// 初始化所有小地圖元素
	InitializeMinimapElements();
	InitializeRoomIcons();
	InitializeRoadPools();

	// 將所有UI元素添加到場景渲染器
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		const auto renderer = scene->GetRoot().lock();

		// 添加背景
		renderer->AddChild(m_MinimapBackground);

		// 添加所有房間圖標
		for (int i = 0; i < 25; ++i)
		{
			renderer->AddChild(m_RoomBoxIcons[i]);
			renderer->AddChild(m_RoomIcons[i]);
		}

		// 添加通道圖標
		for (auto &road : m_HorizontalRoads)
		{
			renderer->AddChild(road);
		}
		for (auto &road : m_VerticalRoads)
		{
			renderer->AddChild(road);
		}
	}

	// 默認顯示面板
	UIPanel::Show();
}

void MinimapPanel::Update()
{
	// 檢查是否應該阻擋輸入
	bool blockInput = ShouldBlockInput();

	// 檢查當前房間是否變化
	CheckCurrentRoomChanged();

	// 更新房間和通道可見性
	UpdateRoomVisibility();
	UpdateRoadVisibility();

	// 更新所有遊戲對象
	for (const auto &gameObject : m_GameObjects)
	{
		if (gameObject)
		{
			gameObject->Update();
		}
	}

	// Debug 功能：按 F1 切換 Debug 視窗
	if (Util::Input::IsKeyDown(Util::Keycode::F1))
	{
		m_EnableDebug = !m_EnableDebug;
	}

	// 顯示 Debug 視窗
	// if (m_EnableDebug)
	// {
	// 	DrawDebugUI();
	// }
}

void MinimapPanel::Show()
{
	UIPanel::Show();

	// 顯示所有相關UI元素
	if (m_MinimapBackground)
		m_MinimapBackground->SetVisible(true);
}

void MinimapPanel::Hide()
{
	UIPanel::Hide();

	// 隱藏所有相關UI元素
	if (m_MinimapBackground)
		m_MinimapBackground->SetVisible(false);

	for (int i = 0; i < 25; ++i)
	{
		SetRoomVisible(i, false);
	}

	for (auto &road : m_HorizontalRoads)
	{
		road->SetVisible(false);
	}
	for (auto &road : m_VerticalRoads)
	{
		road->SetVisible(false);
	}
}

void MinimapPanel::InitializeMinimapElements()
{
	// 創建背景
	m_MinimapBackground = std::make_shared<nGameObject>();
	m_MinimapBackground->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/miniMap/background_miniMap.png"));
	m_MinimapBackground->SetZIndex(88.0f);
	m_MinimapBackground->m_Transform.translation = m_MinimapCenter;
	m_MinimapBackground->m_Transform.scale = glm::vec2(1.0f, 1.0f);
	m_GameObjects.push_back(m_MinimapBackground);
}

void MinimapPanel::InitializeRoomIcons()
{
	for (int i = 0; i < 25; ++i)
	{
		// Box圖標 (16x16) - 使用探索狀態的圖標作為背景
		m_RoomBoxIcons[i] = std::make_shared<nGameObject>();
		m_RoomBoxIcons[i]->SetDrawable(
			ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/miniMap/room_unexplored.png"));
		m_RoomBoxIcons[i]->SetZIndex(88.1f);

		// 房間圖標
		m_RoomIcons[i] = std::make_shared<nGameObject>();
		m_RoomIcons[i]->SetZIndex(88.2f);

		// 預設隱藏
		SetRoomVisible(i, false);

		m_GameObjects.push_back(m_RoomBoxIcons[i]);
		m_GameObjects.push_back(m_RoomIcons[i]);
	}
}

void MinimapPanel::InitializeRoadPools()
{
	// 創建橫向通道對象池 (最多20個)
	m_HorizontalRoads.reserve(20);
	for (int i = 0; i < 20; ++i)
	{
		auto hRoad = std::make_shared<nGameObject>();
		hRoad->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/miniMap/road_0.png"));
		hRoad->SetZIndex(88.15f);
		hRoad->SetVisible(false);
		m_HorizontalRoads.push_back(hRoad);
		m_GameObjects.push_back(hRoad);
	}

	// 創建縱向通道對象池 (最多20個)
	m_VerticalRoads.reserve(20);
	for (int i = 0; i < 20; ++i)
	{
		auto vRoad = std::make_shared<nGameObject>();
		vRoad->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/miniMap/road_1.png"));
		vRoad->SetZIndex(88.15f);
		vRoad->SetVisible(false);
		m_VerticalRoads.push_back(vRoad);
		m_GameObjects.push_back(vRoad);
	}
}

void MinimapPanel::CheckCurrentRoomChanged()
{
	auto dungeonMap = m_DungeonMap.lock();
	if (!dungeonMap)
		return;

	auto currentRoom = dungeonMap->GetCurrentRoom();
	auto lastRoom = m_LastCurrentRoom.lock();

	// 檢測房間變化
	if (currentRoom != lastRoom)
	{
		m_LastCurrentRoom = currentRoom;

		if (currentRoom)
		{
			// 重新計算Starting Room的偏移，讓當前房間位於小地圖中心
			glm::ivec2 currentGridPos = currentRoom->GetMapGridPos();

			// 計算需要的偏移量，讓當前房間對齊小地圖中心
			glm::vec2 idealCurrentPos = m_MinimapCenter; // 當前房間應該在的位置
			glm::vec2 currentRoomMinimapPos = GridToMinimapPos(currentGridPos); // 當前房間在小地圖中的位置

			// 計算偏移量，讓當前房間移動到中心
			m_StartingRoomOffset += (idealCurrentPos - currentRoomMinimapPos);

			UpdateMinimapLayout();
		}
	}
}

void MinimapPanel::UpdateMinimapLayout()
{
	// 更新所有房間圖標的位置
	for (int i = 0; i < 25; ++i)
	{
		int x = i % 5;
		int y = i / 5;
		glm::ivec2 gridPos(x, y);
		glm::vec2 minimapPos = GridToMinimapPos(gridPos);

		m_RoomBoxIcons[i]->m_Transform.translation = minimapPos;
		m_RoomIcons[i]->m_Transform.translation = minimapPos;
	}
}

void MinimapPanel::UpdateRoomVisibility()
{
	auto dungeonMap = m_DungeonMap.lock();
	if (!dungeonMap)
		return;

	for (int i = 0; i < 25; ++i)
	{
		int x = i % 5;
		int y = i / 5;
		glm::ivec2 gridPos(x, y);

		// 計算在小地圖中的位置
		glm::vec2 minimapPos = GridToMinimapPos(gridPos);
		bool inBounds = IsInMinimapBounds(minimapPos);

		if (!inBounds)
		{
			// 超出小地圖範圍，隱藏
			SetRoomVisible(i, false);
			continue;
		}

		// 檢查房間可見性規則
		const auto &roomInfo = dungeonMap->GetRoomInfo(i);
		bool shouldShow = ShouldShowRoom(roomInfo, gridPos);

		SetRoomVisible(i, shouldShow);

		if (shouldShow)
		{
			// 檢查是否為當前房間
			bool isCurrentRoom = false;
			if (auto currentRoom = dungeonMap->GetCurrentRoom())
			{
				glm::ivec2 currentGridPos = currentRoom->GetMapGridPos();
				isCurrentRoom = (currentGridPos.x == x && currentGridPos.y == y);
			}

			// 獲取房間狀態
			RoomState state = RoomState::UNEXPLORED;
			if (roomInfo.room)
			{
				state = roomInfo.room->GetState();
			}

			UpdateRoomIcon(i, state, roomInfo.m_RoomType, isCurrentRoom);
		}
	}
}

void MinimapPanel::UpdateRoadVisibility()
{
	auto dungeonMap = m_DungeonMap.lock();
	if (!dungeonMap)
		return;

	// 清除現有通道
	for (auto &road : m_HorizontalRoads)
		road->SetVisible(false);
	for (auto &road : m_VerticalRoads)
		road->SetVisible(false);

	for (int i = 0; i < 25; ++i)
	{
		int x = i % 5;
		int y = i / 5;
		glm::ivec2 gridPos(x, y);

		const auto &roomInfo = dungeonMap->GetRoomInfo(i);
		if (roomInfo.m_RoomType == RoomType::EMPTY)
			continue;

		// 檢查當前房間的 BoxIcon 是否可見
		if (!m_RoomBoxIcons[i] || !m_RoomBoxIcons[i]->IsVisible())
			continue;

		// 檢查是否在小地圖範圍內
		glm::vec2 minimapPos = GridToMinimapPos(gridPos);
		if (!IsInMinimapBounds(minimapPos))
			continue;

		// 檢查四個方向的連接 (只檢查向右和向下，避免重複)
		if (roomInfo.m_Connections[static_cast<int>(Direction::RIGHT)] && x < 4)
		{
			// 檢查右邊房間的 BoxIcon 是否也可見
			int rightRoomIndex = i + 1; // 右邊房間的索引
			if (m_RoomBoxIcons[rightRoomIndex] && m_RoomBoxIcons[rightRoomIndex]->IsVisible())
			{
				UpdateHorizontalRoad(gridPos, gridPos + glm::ivec2(1, 0), true);
			}
		}
		if (roomInfo.m_Connections[static_cast<int>(Direction::DOWN)] && y < 4)
		{
			// 檢查下方房間的 BoxIcon 是否也可見
			int downRoomIndex = i + 5; // 下方房間的索引 (下一行)
			if (m_RoomBoxIcons[downRoomIndex] && m_RoomBoxIcons[downRoomIndex]->IsVisible())
			{
				UpdateVerticalRoad(gridPos, gridPos + glm::ivec2(0, 1), true);
			}
		}
	}
}

glm::vec2 MinimapPanel::GridToMinimapPos(const glm::ivec2 &gridPos) const
{
	// Starting Room作為參考原點
	glm::vec2 relativePos = glm::vec2(gridPos - m_StartingRoomGridPos);

	// 修復y軸方向：在小地圖中，向下的y應該是負方向
	relativePos.y = -relativePos.y;

	// 轉換為小地圖像素座標 (20像素間距，包含16像素圖標和4像素間隙)
	glm::vec2 minimapPos = m_MinimapCenter + m_StartingRoomOffset + relativePos * 20.0f;

	return minimapPos;
}

bool MinimapPanel::IsInMinimapBounds(const glm::vec2 &minimapPos) const
{
	float halfSize = MINIMAP_SIZE * 0.5f;
	glm::vec2 center = m_MinimapCenter;

	return (minimapPos.x >= center.x - halfSize && minimapPos.x <= center.x + halfSize &&
			minimapPos.y >= center.y - halfSize && minimapPos.y <= center.y + halfSize);
}

bool MinimapPanel::ShouldShowRoom(const RoomInfo &roomInfo, const glm::ivec2 &gridPos)
{
	// 空房間不顯示
	if (roomInfo.m_RoomType == RoomType::EMPTY)
		return false;

	// 已探索的房間顯示
	if (roomInfo.room && roomInfo.room->GetState() != RoomState::UNEXPLORED)
	{
		return true;
	}

	// 未探索房間：只有在相鄰已探索房間時才顯示
	return HasExploredNeighbor(gridPos);
}

bool MinimapPanel::HasExploredNeighbor(const glm::ivec2 &gridPos)
{
	auto dungeonMap = m_DungeonMap.lock();
	if (!dungeonMap)
		return false;

	// 獲取當前位置的房間
	int currentIndex = gridPos.y * 5 + gridPos.x;
	const auto &currentRoomInfo = dungeonMap->GetRoomInfo(currentIndex);

	// 如果當前位置沒有房間，檢查是否有相鄰已探索房間
	if (!currentRoomInfo.room)
	{
		// 檢查四個方向的相鄰房間
		constexpr glm::ivec2 directions[] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

		for (const auto &dir : directions)
		{
			glm::ivec2 neighborPos = gridPos + dir;

			// 邊界檢查
			if (neighborPos.x < 0 || neighborPos.x >= 5 || neighborPos.y < 0 || neighborPos.y >= 5)
				continue;

			int neighborIndex = neighborPos.y * 5 + neighborPos.x;
			const auto &neighborInfo = dungeonMap->GetRoomInfo(neighborIndex);

			// 如果相鄰房間已探索，則顯示此房間
			if (neighborInfo.room && neighborInfo.room->GetState() != RoomState::UNEXPLORED)
			{
				return true;
			}
		}
		return false;
	}

	// 如果當前位置有房間，使用房間的連接關係檢查
	auto currentRoom = currentRoomInfo.room;
	auto connectedNeighbors = currentRoom->GetConnectedNeighbors();

	for (const auto &neighborWeak : connectedNeighbors)
	{
		if (auto neighbor = neighborWeak.lock())
		{
			if (neighbor->GetState() != RoomState::UNEXPLORED)
			{
				return true;
			}
		}
	}

	return false;
}

void MinimapPanel::UpdateRoomIcon(int roomIndex, RoomState state, RoomType type, bool isCurrentRoom)
{
	if (roomIndex < 0 || roomIndex >= 25)
		return;

	auto &roomBoxIcon = m_RoomBoxIcons[roomIndex];
	auto &roomIcon = m_RoomIcons[roomIndex];

	// 更新Box圖標 (背景)
	if (state == RoomState::UNEXPLORED)
	{
		roomBoxIcon->SetDrawable(
			ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/miniMap/room_unexplored.png"));
	}
	else
	{
		roomBoxIcon->SetDrawable(
			ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/miniMap/room_explored.png"));
	}

	// 更新房間圖標
	std::string iconPath = GetRoomIconPath(type);
	roomIcon->SetDrawable(ImagePoolManager::GetInstance().GetImage(iconPath));
}

void MinimapPanel::SetRoomVisible(int roomIndex, bool visible)
{
	if (roomIndex < 0 || roomIndex >= 25)
		return;

	m_RoomBoxIcons[roomIndex]->SetVisible(visible);
	m_RoomIcons[roomIndex]->SetVisible(visible);
}

void MinimapPanel::UpdateHorizontalRoad(const glm::ivec2 &fromGrid, const glm::ivec2 &toGrid, bool visible)
{
	if (!visible)
		return;

	auto road = GetAvailableHorizontalRoad();
	if (!road)
		return;

	// 計算通道位置 (在兩個房間之間)
	glm::vec2 fromPos = GridToMinimapPos(fromGrid);
	glm::vec2 toPos = GridToMinimapPos(toGrid);
	glm::vec2 roadPos = (fromPos + toPos) * 0.5f;

	// 檢查是否在小地圖範圍內
	if (!IsInMinimapBounds(roadPos))
		return;

	road->m_Transform.translation = roadPos;
	road->SetVisible(true);
}

void MinimapPanel::UpdateVerticalRoad(const glm::ivec2 &fromGrid, const glm::ivec2 &toGrid, bool visible)
{
	if (!visible)
		return;

	auto road = GetAvailableVerticalRoad();
	if (!road)
		return;

	// 計算通道位置 (在兩個房間之間)
	glm::vec2 fromPos = GridToMinimapPos(fromGrid);
	glm::vec2 toPos = GridToMinimapPos(toGrid);
	glm::vec2 roadPos = (fromPos + toPos) * 0.5f;

	// 檢查是否在小地圖範圍內
	if (!IsInMinimapBounds(roadPos))
		return;

	road->m_Transform.translation = roadPos;
	road->SetVisible(true);
}

std::shared_ptr<nGameObject> MinimapPanel::GetAvailableHorizontalRoad()
{
	for (auto &road : m_HorizontalRoads)
	{
		if (!road->IsVisible())
		{
			return road;
		}
	}
	return nullptr;
}

std::shared_ptr<nGameObject> MinimapPanel::GetAvailableVerticalRoad()
{
	for (auto &road : m_VerticalRoads)
	{
		if (!road->IsVisible())
		{
			return road;
		}
	}
	return nullptr;
}

std::string MinimapPanel::GetRoomIconPath(RoomType type) const
{
	switch (type)
	{
	case RoomType::STARTING:
		return RESOURCE_DIR "/UI/miniMap/startingRoom_icon.png";
	case RoomType::MONSTER:
		return RESOURCE_DIR "/UI/miniMap/mosterRoom_icon.png"; // 使用探索房間圖標
	case RoomType::PORTAL:
		return RESOURCE_DIR "/UI/miniMap/portalRoom_icon.png";
	case RoomType::BOSS:
		return RESOURCE_DIR "/UI/miniMap/bossRoom_icon.png"; // 暫時使用探索房間圖標
	case RoomType::CHEST:
		return RESOURCE_DIR "/UI/miniMap/chestRoom_icon.png";
	case RoomType::SPECIAL:
		return RESOURCE_DIR "/UI/miniMap/specialRoom_icon.png";
	default:
		return RESOURCE_DIR "/UI/miniMap/mosterRoom_icon.png";
	}
}

void MinimapPanel::DrawDebugUI()
{
	// 初始化 DebugPos（只做一次）
	if (!m_DebugInitialized)
	{
		m_DebugPos = m_MinimapCenter;
		m_DebugInitialized = true;
	}

	// 根據使用者輸入更新位置
	if (m_PosChangedByInput)
	{
		m_MinimapCenter = m_DebugPos;
		UpdateAllElementsPosition();
		m_PosChangedByInput = false;
	}

	// 彈出調整視窗
	ImGui::Begin("Minimap Position Debug");

	// 位置調整
	ImGui::InputFloat("Minimap Center X", &m_DebugPos.x, 1.0f);
	if (ImGui::IsItemDeactivatedAfterEdit())
		m_PosChangedByInput = true;

	ImGui::InputFloat("Minimap Center Y", &m_DebugPos.y, 1.0f);
	if (ImGui::IsItemDeactivatedAfterEdit())
		m_PosChangedByInput = true;

	// 不要每幀覆蓋 m_DebugPos，否則用戶輸入會被吃掉
	if (!m_PosChangedByInput && !ImGui::IsAnyItemActive())
		m_DebugPos = m_MinimapCenter;

	// 顯示當前位置狀態
	ImGui::LabelText("Current Center X: ", "%.3f", m_MinimapCenter.x);
	ImGui::LabelText("Current Center Y: ", "%.3f", m_MinimapCenter.y);

	// 快速預設位置按鈕
	if (ImGui::Button("Top Right"))
	{
		m_DebugPos = glm::vec2(500.0f, -250.0f);
		m_PosChangedByInput = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Top Left"))
	{
		m_DebugPos = glm::vec2(-500.0f, -250.0f);
		m_PosChangedByInput = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Bottom Right"))
	{
		m_DebugPos = glm::vec2(500.0f, 250.0f);
		m_PosChangedByInput = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Bottom Left"))
	{
		m_DebugPos = glm::vec2(-500.0f, 250.0f);
		m_PosChangedByInput = true;
	}

	ImGui::Text("Press F1 to toggle this debug window");

	ImGui::End();
}

void MinimapPanel::UpdateAllElementsPosition()
{
	// 更新背景位置
	if (m_MinimapBackground)
	{
		m_MinimapBackground->m_Transform.translation = m_MinimapCenter;
	}

	// 更新所有房間圖標位置
	UpdateMinimapLayout();
}
