# DungeonRoom 連接系統重構

## 🔗 功能概述

為了更好地支援小地圖功能，對 DungeonRoom 進行了重構，添加了房間間的連接關係管理。現在每個房間都會記錄四個方向的相鄰房間引用和連接狀態。

## 📋 主要改動

### 1. **新增 RoomConnection 結構體**

```cpp
struct RoomConnection {
    std::array<std::weak_ptr<DungeonRoom>, 4> neighbors;  // 四個方向的相鄰房間
    std::array<bool, 4> hasConnection = {false, false, false, false};  // 是否有通道連接

    // 輔助方法
    std::weak_ptr<DungeonRoom> GetNeighbor(Direction dir) const;
    void SetNeighbor(Direction dir, const std::weak_ptr<DungeonRoom>& room);
    bool HasConnection(Direction dir) const;
    void SetConnection(Direction dir, bool connected);
    std::vector<std::weak_ptr<DungeonRoom>> GetConnectedNeighbors() const;
    std::vector<Direction> GetConnectedDirections() const;
};
```

### 2. **DungeonRoom 新增連接管理方法**

```cpp
class DungeonRoom : public Room {
    // 房間連接管理
    const RoomConnection& GetRoomConnection() const;
    void SetNeighborRoom(Direction dir, const std::weak_ptr<DungeonRoom>& neighbor, bool hasConnection = false);
    std::weak_ptr<DungeonRoom> GetNeighborRoom(Direction dir) const;
    bool HasConnectionToDirection(Direction dir) const;
    std::vector<std::weak_ptr<DungeonRoom>> GetConnectedNeighbors() const;
    std::vector<Direction> GetConnectedDirections() const;

private:
    RoomConnection m_RoomConnection;
};
```

### 3. **DungeonMap 添加連接設置邏輯**

```cpp
void DungeonMap::SetupRoomConnections() {
    // 在所有房間創建完成後，設置房間間的連接關係
    for (int i = 0; i < std::size(m_RoomInfo); ++i) {
        if (m_RoomInfo[i].m_RoomType == RoomType::EMPTY || !m_RoomInfo[i].room) continue;

        // 檢查四個方向的相鄰房間並設置連接
        for (Direction dir : ALL_DIRECTIONS) {
            // 計算相鄰位置並設置連接關係
        }
    }
}
```

### 4. **小地圖優化**

更新了 `HasExploredNeighbor` 方法：

```cpp
bool MinimapPanel::HasExploredNeighbor(const glm::ivec2 &gridPos) {
    // 使用房間的連接關係而不是簡單的網格檢查
    auto currentRoom = currentRoomInfo.room;
    auto connectedNeighbors = currentRoom->GetConnectedNeighbors();

    for (const auto& neighborWeak : connectedNeighbors) {
        if (auto neighbor = neighborWeak.lock()) {
            if (neighbor->GetState() != RoomState::UNEXPLORED) {
                return true;
            }
        }
    }
}
```

## 🎯 優化效果

### **原來的問題**

- 小地圖顯示所有相鄰房間，不管是否有通道連接
- 複雜的座標計算來找相鄰房間
- StartingRoom 和 PortalRoom 只有一個通道但顯示了不相關的房間

### **現在的優勢**

- ✅ **精確連接**：只顯示有通道連接的相鄰房間
- ✅ **高效查詢**：直接通過 weak_ptr 訪問相鄰房間
- ✅ **邏輯一致**：StartingRoom 和 PortalRoom 只顯示實際連接的房間
- ✅ **代碼簡化**：GetNeighborRooms 方法大幅簡化

## 🔄 使用流程

1. **房間創建**：`DungeonMap::Start()` 創建所有房間
2. **設置連接**：`DungeonMap::SetupRoomConnections()` 建立房間間的連接關係
3. **小地圖使用**：小地圖通過 `GetConnectedNeighbors()` 獲取真正連接的房間

## 📍 應用場景

- **小地圖顯示**：只顯示有通道連接的房間
- **AI 導航**：敵人可以沿著連接的路徑移動
- **遊戲邏輯**：房間解鎖、任務進度等可以基於連接關係
- **調試工具**：可以輕鬆查看房間的連接狀態

---

**注意**：此重構保持了原有的 DungeonMap 接口，只是內部實現更加高效和精確。
