//
// Created by QuzzS on 2025/2/28.
//

#include "Camera.hpp"

#include "Util/Time.hpp"
#include "Override/nGameObject.hpp"

Camera::Camera(
	const std::vector<std::shared_ptr<nGameObject>> &pivotChildren
	) : m_Children(pivotChildren)
{
	m_CameraWorldCoord.translation = {0.0f,0.0f};
	m_CameraWorldCoord.rotation = 0.0f;
	m_CameraWorldCoord.scale = glm::vec2{2.5f};
}

void Camera::onInputReceived(const std::set<char>& keys) {
	if (keys.count('I')) ZoomCamera(1);
	if (keys.count('K')) ZoomCamera(-1);
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

void Camera::SetFollowTarget(const std::shared_ptr<nGameObject> &target) {
	m_FollowTarget = target;
}

void Camera::ZoomCamera(const float zoomLevel)
{
	const float minZoom = 0.0f;  // 最小縮放值
	const float maxZoom = 100.0f;  // 最大縮放值

	glm::vec2 newScale = m_CameraWorldCoord.scale + glm::vec2(1.0f, 1.0f) * zoomLevel * (Util::Time::GetDeltaTimeMs()/1000.0f);
	newScale.x = std::clamp(newScale.x, minZoom, maxZoom);
	newScale.y = std::clamp(newScale.y, minZoom, maxZoom);

	m_CameraWorldCoord.scale = newScale;
}

void Camera::RotateCamera(const float radian) // radian 是PI， degree是°
{
	m_CameraWorldCoord.rotation -= radian * Util::Time::GetDeltaTimeMs() / 1000;
}

void Camera::AddChild(const std::shared_ptr<nGameObject> &child) {
	m_Children.push_back(child);
	// 若為負的會影響物件池内的物件
	glm::vec2 absScale = {std::abs(child->m_Transform.scale.x), std::abs(child->m_Transform.scale.y)};
	// 如果尚未設置初始縮放
	if (!child->isSetInitialScale()) {
		child->SetInitialScale(absScale);
		child->SetInitialScaleSet(true);
	}
}

void Camera::RemoveChild(const std::shared_ptr<nGameObject>& child) {
	m_Children.erase(
		std::remove(m_Children.begin(), m_Children.end(), child),
		m_Children.end()
	);
}

void Camera::AddChildren(
		const std::vector<std::shared_ptr<nGameObject>> &children) {
	m_Children.reserve(m_Children.size() + children.size());
	m_Children.insert(m_Children.end(), children.begin(), children.end());
}

void Camera::Update() {
	if (auto target = m_FollowTarget.lock()) {
		m_CameraWorldCoord.translation = target->m_WorldCoord;
	}

	// 對每個Object調位置
	for (const auto& child : m_Children)
	{
		// TODO:需要变数调解？
		if (NotShouldBeVisible(child))
		{
			child->SetVisible(false);
			continue;
		}
		if (child->GetZIndexType() != UI)
		{
			child->SetVisible(true);
		}
		UpdateChildViewportPosition(child);
	}
}

void Camera::UpdateChildViewportPosition(const std::shared_ptr<nGameObject> &child)
{
	//變更坐標軸
	// child->SetPivot(m_CameraWorldCoord.translation - child->m_WorldCoord);//成功 - 跟著鏡頭縮放旋轉 但是改變Object Pivot以後槍旋轉點、子彈從槍口發射可能會有問題
	//Obejct窗口位置 = (Object世界坐標 - Camera世界坐標) * 縮放倍率
	child->m_Transform.translation = (child->m_WorldCoord - m_CameraWorldCoord.translation) * m_CameraWorldCoord.scale;

	//動態調整ZIndex
	UpdateZIndex(child);

	glm::vec2 initialScale = child->GetInitialScale();
	// std::copysign(第一個參數：大小, 第二個參數：正負號)
	child->m_Transform.scale = glm::vec2(
		initialScale.x * std::copysign(m_CameraWorldCoord.scale.x, child->m_Transform.scale.x),
		initialScale.y * std::copysign(m_CameraWorldCoord.scale.y, child->m_Transform.scale.y)
	);
}

void Camera::UpdateZIndex(const std::shared_ptr<nGameObject> &child) const
{
	const auto ZIndexLayer = child->GetZIndexType();
	if (ZIndexLayer == ZIndexType::CUSTOM) return; //自動跳過動態調整 --例如跟隨

	// 特殊處理UI層
	if (ZIndexLayer == ZIndexType::UI)
	{
		const auto ZIndexNum = child->GetZIndex();
		if (child->GetZIndex() < ZIndexType::UI) child->SetZIndex(ZIndexType::UI + ZIndexNum * 0.2f);
		return;
	}

	// 動態處理其他層
	if (m_MapYSize != 0.0f)
	{
		// 根據物體Y座標在該區間的相對位置計算最終ZIndex
		const float relativeY = (m_MapYSize/2.0f - child->m_WorldCoord.y + child->GetImageSize().y/2.0f) / m_MapYSize;
		child->SetZIndex(static_cast<float>(ZIndexLayer) + (relativeY * 20.0f));
	}
}

bool Camera::NotShouldBeVisible(const std::shared_ptr<nGameObject> &child) const
{
	glm::vec2 delta = child->m_WorldCoord - m_CameraWorldCoord.translation;
	float roomRegionSize = 16 * 35;
	float threshold = roomRegionSize;
	return std::abs(delta.x) >  threshold || std::abs(delta.y) > threshold;
}





