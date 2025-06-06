//
// Created by QuzzS on 2025/2/28.
//

#include "Camera.hpp"

#include "ObserveManager/EventManager.hpp"
#include "Override/nGameObject.hpp"
#include "Structs/EventInfo.hpp"
#include "Structs/TakeDamageEventInfo.hpp"
#include "Util/Time.hpp"


Camera::Camera(const std::vector<std::shared_ptr<nGameObject>> &pivotChildren) :
	m_Children(pivotChildren), m_RandomGenerator(std::random_device{}())
{
	m_CameraWorldCoord.translation = {0.0f, 0.0f};
	m_CameraWorldCoord.rotation = 0.0f;
	m_CameraWorldCoord.scale = glm::vec2{2.5f};

	// 初始化事件監聽器
	InitializeEventListeners();
}

Camera::~Camera()
{
	// 清理事件監聽器，防止懸空指針
	auto &eventManager = EventManager::GetInstance();
	eventManager.Unsubscribe<CameraShakeEvent>(m_CameraShakeListenerID);
}

void Camera::onInputReceived(const std::set<char> &keys)
{
	if (keys.count('I'))
		ZoomCamera(1);
	if (keys.count('K'))
		ZoomCamera(-1);
}


void Camera::MoveCamera(const glm::vec2 &deltaDisplacement)
{
	m_CameraWorldCoord.translation += deltaDisplacement;
	// TODO:限制範圍 應該要圓形的 可能需要變數 範圍還沒限制到
	// const float u = beaconCoord.x * std::sqrt(1 - beaconCoord.y * beaconCoord.y / 2);
	// const float v = beaconCoord.y * std::sqrt(1 - beaconCoord.x * beaconCoord.x / 2);
	// if (m_CameraWorldCoord.translation.x > beaconCoord.x + u) {m_CameraWorldCoord.translation.x = beaconCoord.x + u;}
	// if (m_CameraWorldCoord.translation.y > beaconCoord.y + v) {m_CameraWorldCoord.translation.y = beaconCoord.y + v;}
	// if (m_CameraWorldCoord.translation.x < beaconCoord.x + u) {m_CameraWorldCoord.translation.x = beaconCoord.x + u;}
	// if (m_CameraWorldCoord.translation.y < beaconCoord.y + v) {m_CameraWorldCoord.translation.y = beaconCoord.y + v;}
}

void Camera::SetFollowTarget(const std::shared_ptr<nGameObject> &target) { m_FollowTarget = target; }

void Camera::ZoomCamera(const float zoomLevel)
{
	const float minZoom = 0.0f; // 最小縮放值
	const float maxZoom = 100.0f; // 最大縮放值

	glm::vec2 newScale =
		m_CameraWorldCoord.scale + glm::vec2(1.0f, 1.0f) * zoomLevel * (Util::Time::GetDeltaTimeMs() / 1000.0f);
	newScale.x = std::clamp(newScale.x, minZoom, maxZoom);
	newScale.y = std::clamp(newScale.y, minZoom, maxZoom);

	m_CameraWorldCoord.scale = newScale;
}

void Camera::RotateCamera(const float radian) // radian 是PI， degree是°
{
	m_CameraWorldCoord.rotation -= radian * Util::Time::GetDeltaTimeMs() / 1000;
}

void Camera::AddChild(const std::shared_ptr<nGameObject> &child)
{
	if (child == nullptr)
		return;
	m_Children.push_back(child);
	// 若為負的會影響物件池内的物件
	glm::vec2 absScale = {std::abs(child->m_Transform.scale.x), std::abs(child->m_Transform.scale.y)};
	// 如果尚未設置初始縮放
	if (!child->isSetInitialScale())
	{
		child->SetInitialScale(absScale);
		child->SetInitialScaleSet(true);
	}
}

void Camera::RemoveChild(const std::shared_ptr<nGameObject> &child)
{
	m_Children.erase(std::remove(m_Children.begin(), m_Children.end(), child), m_Children.end());
}

void Camera::AddChildren(const std::vector<std::shared_ptr<nGameObject>> &children)
{
	m_Children.reserve(m_Children.size() + children.size());
	m_Children.insert(m_Children.end(), children.begin(), children.end());
}

void Camera::MarkForRemoval(const std::shared_ptr<nGameObject> &child) { m_ToRemoveList.emplace_back(child); }


bool Camera::FindChild(const std::shared_ptr<nGameObject> &child)
{
	return std::find(m_Children.begin(), m_Children.end(), child) != m_Children.end();
}


void Camera::Update()
{
	static int count = 0;

	// 更新抖動效果
	m_ShakeTimer.Update();

	if (auto target = m_FollowTarget.lock())
	{
		m_CameraWorldCoord.translation = target->m_WorldCoord;
	}

	// 應用抖動偏移
	if (m_IsShaking)
	{
		m_CameraWorldCoord.translation += m_ShakeOffset;
	}
	int i = 0;
	// 對每個Object調位置
	for (const auto &child : m_Children)
	{
		if (!child)
			continue;
		// IsInsideWindow來專門管理是否在視窗内
		if (NotShouldBeVisible(child))
		{
			child->SetIsInsideWindow(false);
		}
		else
		{
			child->SetIsInsideWindow(true);
		}

		// 判斷是否顯示
		child->SetVisible(child->IsInsideWindow() && child->IsControlVisible());
		if (!child->IsInsideWindow())
			continue; // 沒顯示就不移動了
		child->Update();
		UpdateChildViewportPosition(child);
		i++;
	}

	if (count == 100)
	{
		// LOG_DEBUG("Camera::Update {} {} ", m_Children.size(), i);
		count = 0;
	}
	count++;

	// 延後處理移除
	for (auto &weakObject : m_ToRemoveList)
	{
		if (auto obj = weakObject.lock())
		{
			RemoveChild(obj);
		}
	}
	m_ToRemoveList.clear();

	// 延後處理加入的child
	for (auto &weakObject : m_ToAddList)
	{
		if (auto obj = weakObject.lock())
		{
			AddChild(obj);
		}
	}
	m_ToAddList.clear();
}

void Camera::UpdateChildViewportPosition(const std::shared_ptr<nGameObject> &child)
{
	// 變更坐標軸
	//  child->SetPivot(m_CameraWorldCoord.translation - child->m_WorldCoord);//成功 - 跟著鏡頭縮放旋轉 但是改變Object
	//  Pivot以後槍旋轉點、子彈從槍口發射可能會有問題
	// Obejct窗口位置 = (Object世界坐標 - Camera世界坐標) * 縮放倍率
	child->m_Transform.translation = (child->m_WorldCoord - m_CameraWorldCoord.translation) * m_CameraWorldCoord.scale;

	// 動態調整ZIndex
	UpdateZIndex(child);

	glm::vec2 initialScale = child->GetInitialScale();
	// std::copysign(第一個參數：大小, 第二個參數：正負號)
	child->m_Transform.scale =
		glm::vec2(initialScale.x * std::copysign(m_CameraWorldCoord.scale.x, child->m_Transform.scale.x),
				  initialScale.y * std::copysign(m_CameraWorldCoord.scale.y, child->m_Transform.scale.y));
}

void Camera::UpdateZIndex(const std::shared_ptr<nGameObject> &child) const
{
	const auto ZIndexLayer = child->GetZIndexType();
	if (ZIndexLayer == ZIndexType::CUSTOM)
		return; // 自動跳過動態調整 --例如跟隨

	// 特殊處理UI層
	if (ZIndexLayer == ZIndexType::UI)
	{
		const auto ZIndexNum = child->GetZIndex();
		if (child->GetZIndex() < ZIndexType::UI)
			child->SetZIndex(ZIndexType::UI + ZIndexNum * 0.2f);
		return;
	}

	// 動態處理其他層
	if (m_MapYSize != 0.0f)
	{
		// 根據物體Y座標在該區間的相對位置計算最終ZIndex
		const float relativeY =
			(m_MapYSize / 2.0f - child->m_WorldCoord.y + child->GetImageSize().y / 2.0f) / m_MapYSize;
		child->SetZIndex(static_cast<float>(ZIndexLayer) + (relativeY * 20.0f));
	}
}

bool Camera::NotShouldBeVisible(const std::shared_ptr<nGameObject> &child) const
{
	glm::vec2 delta = child->m_WorldCoord - m_CameraWorldCoord.translation;
	float roomRegionSize = 16 * 35;
	float threshold = roomRegionSize;
	return std::abs(delta.x) > threshold || std::abs(delta.y) > threshold;
}

void Camera::StartShake(float duration, float intensity)
{
	m_ShakeIntensity = intensity;
	m_IsShaking = true;

	// 設定更新回調：每幀產生隨機抖動
	m_ShakeTimer.SetOnUpdate(
		[this](float progress)
		{
			// 抖動強度隨時間遞減（使用反向進度讓強度從最大到0）
			float currentIntensity = m_ShakeIntensity * (1.0f - progress);

			// 產生隨機偏移
			float randomX = m_RandomDistribution(m_RandomGenerator) * currentIntensity;
			float randomY = m_RandomDistribution(m_RandomGenerator) * currentIntensity;
			m_ShakeOffset = glm::vec2(randomX, randomY);
		});

	// 設定完成回調：停止抖動
	m_ShakeTimer.SetOnFinished([this]() { StopShake(); });

	m_ShakeTimer.SetAndStart(duration);
}

void Camera::StopShake()
{
	m_IsShaking = false;
	m_ShakeOffset = glm::vec2(0.0f);
	m_ShakeTimer.Stop();
	m_ShakeTimer.ClearCallbacks();
}

bool Camera::IsShaking() const { return m_IsShaking; }

void Camera::InitializeEventListeners()
{
	auto &eventManager = EventManager::GetInstance();

	// 只監聽Camera抖動事件，並保存監聽器ID
	m_CameraShakeListenerID = eventManager.Subscribe<CameraShakeEvent>(
		[this](const CameraShakeEvent &event) { StartShake(event.duration, event.intensity); });
}
