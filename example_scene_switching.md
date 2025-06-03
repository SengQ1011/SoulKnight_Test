# 新的場景切換系統使用指南

## 方案一：外部直接設定場景切換

現在您可以從任何地方直接設定下一個要切換的場景，不再需要依賴場景內部的 `Change()` 方法。

### 1. 基本使用方法

```cpp
// 從任何地方直接切換到主選單
SceneManager::GetInstance().SetNextScene(Scene::SceneType::Menu);

// 從任何地方直接切換到地牢
SceneManager::GetInstance().SetNextScene(Scene::SceneType::Dungeon);

// 從任何地方直接切換到大廳
SceneManager::GetInstance().SetNextScene(Scene::SceneType::Lobby);
```

### 2. UI 按鈕中的使用

在 `PausePanel.cpp` 中的示範：

```cpp
// MenuButton - 返回主選單
std::function<void()> menu_function = []() {
    SceneManager::GetInstance().SetNextScene(Scene::SceneType::Menu);
};

// 如果要進入地牢
std::function<void()> dungeon_function = []() {
    SceneManager::GetInstance().SetNextScene(Scene::SceneType::DungeonLoad);
};
```

### 3. 傳送門系統中的使用

在 `InteractableComponent.cpp` 中的改進：

```cpp
case InteractableType::PORTAL:
{
    m_InteractionCallback = [](const std::shared_ptr<Character> &interactor, const std::shared_ptr<nGameObject> &target)
    {
        const auto &scene = SceneManager::GetInstance().GetCurrentScene().lock();
        if (!scene) return;

        // 根據當前場景決定傳送門目標
        switch (scene->GetSceneType()) {
            case Scene::SceneType::Lobby:
                // 大廳傳送門進入地牢
                SceneManager::GetInstance().SetNextScene(Scene::SceneType::DungeonLoad);
                break;
            case Scene::SceneType::Dungeon:
                // 地牢傳送門進入下一關
                SceneManager::GetInstance().SetNextScene(Scene::SceneType::DungeonLoad);
                break;
            default:
                // 預設行為
                scene->SetIsChange(true);
                break;
        }
    };
    break;
}
```

### 4. 條件式場景切換

```cpp
// 根據遊戲狀態決定場景切換
void HandleGameEnd(bool playerWon) {
    if (playerWon) {
        SceneManager::GetInstance().SetNextScene(Scene::SceneType::Result);
    } else {
        SceneManager::GetInstance().SetNextScene(Scene::SceneType::Menu);
    }
}

// 根據關卡進度決定場景
void HandlePortalInteraction(int currentStage) {
    if (currentStage >= 5) {
        SceneManager::GetInstance().SetNextScene(Scene::SceneType::Result);
    } else {
        SceneManager::GetInstance().SetNextScene(Scene::SceneType::DungeonLoad);
    }
}
```

### 5. 場景切換的優先級

新的系統有以下優先級：

1. **最高優先級**：外部透過 `SetNextScene()` 設定的場景
2. **次要優先級**：場景內部的 `Change()` 方法返回的場景

```cpp
// 在 SceneManager::ChangeCurrentScene() 中：
void SceneManager::ChangeCurrentScene() {
    // 優先檢查外部設定的場景切換
    if (m_NextSceneType == Scene::SceneType::Null) {
        m_NextSceneType = m_CurrentScene->Change();
    }

    // 其餘切換邏輯...
}
```

## 優點

### 1. 高度靈活性

- 可以從任何地方（UI、傳送門、事件處理器等）直接設定場景切換
- 不需要修改場景內部的 `Change()` 方法
- 支援條件式和動態場景切換

### 2. 清晰的責任分離

- UI 元素可以直接處理自己的場景切換邏輯
- 傳送門可以根據自己的配置決定目標場景
- 場景內部的 `Change()` 方法作為預設備案

### 3. 易於擴展

- 未來可以輕鬆添加新的場景切換觸發點
- 可以實作複雜的場景切換邏輯
- 支援場景切換的優先級管理

## 使用建議

1. **對於 UI 按鈕**：直接使用 `SetNextScene()`
2. **對於傳送門**：根據當前場景或傳送門配置使用 `SetNextScene()`
3. **對於遊戲邏輯**：在特定條件滿足時使用 `SetNextScene()`
4. **對於預設行為**：保留場景的 `Change()` 方法作為備案
