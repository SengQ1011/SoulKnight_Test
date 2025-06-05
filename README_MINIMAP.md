# 小地圖系統 (MinimapPanel) 實作說明

## 🗺️ 功能概述

小地圖系統提供了一個 128×128 像素的迷你地圖 UI，顯示玩家在 5×5 房間網格中的位置和探索進度。

## 📋 核心特性

### 1. **視覺規格**

- **小地圖尺寸**: 128×128 像素
- **房間圖標**: 16×16 像素 (BoxIcon + RoomIcon)
- **通道尺寸**:
  - 橫向: 8×4 像素 (`road_0.png`)
  - 縱向: 4×8 像素 (`road_1.png`)
- **房間間距**: 20 像素 (16 像素圖標 + 4 像素間隙)

### 2. **顯示邏輯**

- ✅ **中心對齊**: 當前房間始終在小地圖中心
- ✅ **探索可見性**: 只顯示已探索房間和相鄰未探索房間
- ✅ **動態更新**: 當前房間變化時自動重新定位
- ✅ **範圍限制**: 類似 Camera，超出 128×128 範圍的元素自動隱藏

### 3. **房間狀態視覺化**

- 🟫 **未探索**: `room_unexplored.png` + 半透明圖標
- 🟩 **已探索**: `room_explored.png` + 正常圖標
- ⭐ **當前房間**: 1.3 倍放大顯示
- 🚪 **通道**: 只有已探索房間間的連接才會顯示

## 🏗️ 架構設計

### 類別結構

```cpp
class MinimapPanel : public UIPanel {
    // 核心組件
    std::weak_ptr<DungeonMap> m_DungeonMap;
    std::shared_ptr<nGameObject> m_MinimapBackground;

    // 房間圖標陣列 (5x5 = 25個)
    std::array<std::shared_ptr<nGameObject>, 25> m_RoomBoxIcons;
    std::array<std::shared_ptr<nGameObject>, 25> m_RoomIcons;

    // 通道對象池
    std::vector<std::shared_ptr<nGameObject>> m_HorizontalRoads;
    std::vector<std::shared_ptr<nGameObject>> m_VerticalRoads;
};
```

### 座標轉換系統

```cpp
glm::vec2 GridToMinimapPos(const glm::ivec2& gridPos) const {
    // Starting Room作為參考原點
    glm::vec2 relativePos = glm::vec2(gridPos - m_StartingRoomGridPos);

    // 修復y軸方向：在小地圖中，向下的y應該是負方向
    relativePos.y = -relativePos.y;

    // 轉換為小地圖像素座標
    glm::vec2 minimapPos = m_MinimapCenter + m_StartingRoomOffset +
                          relativePos * 20.0f;

    return minimapPos;
}
```

**座標系統說明**：

- 🎯 **網格座標**：DungeonMap 使用標準的 (x, y) 網格座標，其中 y++ 表示向下
- 🖼️ **螢幕座標**：小地圖使用螢幕座標系統，其中 y-- 表示向上
- 🔄 **轉換處理**：通過 `relativePos.y = -relativePos.y` 來修復 y 軸方向差異

## 📂 資源文件

### 小地圖資源位置: `Resources/UI/miniMap/`

- `background_miniMap.png` - 128×128 背景
- `room_unexplored.png` - 未探索房間背景
- `room_explored.png` - 已探索房間背景
- `road_0.png` - 橫向通道 (8×4)
- `road_1.png` - 縱向通道 (4×8)

### 房間類型圖標

- `startingRoom_icon.png` - 起始房間
- `portalRoom_icon.png` - 傳送門房間
- `chestRoom_icon.png` - 寶箱房間
- `specialRoom_icon.png` - 特殊房間

## 🔧 整合方式

### 1. 在 GameHUDPanel 中整合

```cpp
// 頭文件
class GameHUDPanel : public UIPanel {
protected:
    std::shared_ptr<MinimapPanel> m_MinimapPanel;
    std::shared_ptr<DungeonMap> m_DungeonMap;
};

// 構造函數
GameHUDPanel::GameHUDPanel(
    const std::shared_ptr<HealthComponent>& healthComp,
    const std::shared_ptr<WalletComponent>& walletComp,
    const std::shared_ptr<DungeonMap>& dungeonMap)
    : m_DungeonMap(dungeonMap) {}

// Start方法中初始化
if (m_DungeonMap) {
    m_MinimapPanel = std::make_shared<MinimapPanel>(m_DungeonMap);
    m_MinimapPanel->Start();
}
```

### 2. 在 DungeonScene 中傳入 DungeonMap

```cpp
// 修改GameHUDPanel的創建
const auto gameHUDPanel = std::make_shared<GameHUDPanel>(
    m_Player->GetComponent<HealthComponent>(ComponentType::HEALTH),
    m_Player->GetComponent<WalletComponent>(ComponentType::WALLET),
    m_Map  // 傳入DungeonMap
);
```

## 🎯 核心算法

### 房間可見性判斷

```cpp
bool ShouldShowRoom(const RoomInfo& roomInfo, const glm::ivec2& gridPos) {
    // 空房間不顯示
    if (roomInfo.m_RoomType == RoomType::EMPTY) return false;

    // 已探索的房間顯示
    if (roomInfo.room && roomInfo.room->GetState() != RoomState::UNEXPLORED) {
        return true;
    }

    // 未探索房間：只有在相鄰已探索房間時才顯示
    return HasExploredNeighbor(gridPos);
}
```

### 動態重新定位

```cpp
void CheckCurrentRoomChanged() {
    // 檢測房間變化
    if (currentRoom != lastRoom) {
        // 計算偏移量，讓當前房間移動到中心
        glm::vec2 idealCurrentPos = m_MinimapCenter;
        glm::vec2 currentRoomMinimapPos = GridToMinimapPos(currentGridPos);
        m_StartingRoomOffset += (idealCurrentPos - currentRoomMinimapPos);

        UpdateMinimapLayout();
    }
}
```

## 🔄 更新流程

1. **CheckCurrentRoomChanged()** - 檢測當前房間變化
2. **UpdateRoomVisibility()** - 更新房間可見性和狀態
3. **UpdateRoadVisibility()** - 更新通道可見性
4. **UpdateMinimapLayout()** - 更新所有元素位置

## 📍 位置配置

```cpp
// 小地圖在螢幕上的位置
glm::vec2 m_MinimapCenter = glm::vec2(500.0f, -250.0f);

// 初始房間在5x5格子中的位置 (2,2為中心)
glm::ivec2 m_StartingRoomGridPos = glm::ivec2(2, 2);
```

## 🚀 使用方式

1. 確保 DungeonMap 實現了`GetRoomInfo(int index)`方法
2. 在 GameHUDPanel 構造時傳入 DungeonMap
3. 小地圖會自動跟隨當前房間變化並更新顯示
4. 支援 Show/Hide 操作，配合 UIManager 使用

---

**注意**: 此實作需要 DungeonMap 提供房間信息訪問接口，並且假設房間網格為 5×5 固定大小。
