//
// Created by QuzzS on 2025/2/28.
//

#include "Camera.hpp"

Camera::Camera(
	const std::vector<std::shared_ptr<nGameObject>> &pivotChildren
	) : m_RelativePivotChildren(pivotChildren)
{
	m_CameraWorldCoord.translation = {0.0f,0.0f};
	m_CameraWorldCoord.rotation = 0.0f;
	m_CameraWorldCoord.scale = {1.0f,1.0f};
}

// void Camera::MoveCamera(const glm::vec2 &displacement)
// {
// 	//加位移變化量
// 	m_CameraWorldCoord.translation += displacement * Util::Time::GetDeltaTimeMs() / 5.0f / std::sqrt(displacement.x * displacement.x + displacement.y * displacement.y);
// }

void Camera::MoveCamera(const glm::vec2 &beaconCoord)
{
	//加位移變化量
	glm::vec2 UnitVector = beaconCoord / std::sqrt(beaconCoord.x * beaconCoord.x + beaconCoord.y * beaconCoord.y);
	m_CameraWorldCoord.translation += UnitVector * Util::Time::GetDeltaTimeMs() / 5.0f;
	// TODO:限制範圍 應該要圓形的 可能需要變數 範圍還沒限制到
	const float u = beaconCoord.x * std::sqrt(1 - beaconCoord.y * beaconCoord.y / 2);
	const float v = beaconCoord.y * std::sqrt(1 - beaconCoord.x * beaconCoord.x / 2);
	// if (m_CameraWorldCoord.translation.x > beaconCoord.x + u) {m_CameraWorldCoord.translation.x = beaconCoord.x + u;}
	// if (m_CameraWorldCoord.translation.y > beaconCoord.y + v) {m_CameraWorldCoord.translation.y = beaconCoord.y + v;}
	// if (m_CameraWorldCoord.translation.x < beaconCoord.x + u) {m_CameraWorldCoord.translation.x = beaconCoord.x + u;}
	// if (m_CameraWorldCoord.translation.y < beaconCoord.y + v) {m_CameraWorldCoord.translation.y = beaconCoord.y + v;}
}

void Camera::ZoomCamera(const float zoomLevel)
{
	m_CameraWorldCoord.scale += glm::vec2(1.0f,1.0f) * zoomLevel * Util::Time::GetDeltaTimeMs() / 1000.0f;
}

void Camera::RotateCamera(const float radian) // radian 是PI， degree是°
{
	m_CameraWorldCoord.rotation -= radian * Util::Time::GetDeltaTimeMs() / 1000;
}

void Camera::AddRelativePivotChild(const std::shared_ptr<nGameObject> &child) {
	m_RelativePivotChildren.push_back(child);
}

void Camera::RemoveRelativePivotChild(std::shared_ptr<nGameObject> child) {
	m_RelativePivotChildren.erase(std::remove(m_RelativePivotChildren.begin(), m_RelativePivotChildren.end(), child),
									 m_RelativePivotChildren.end());
}

void Camera::AddRelativePivotChildren(
		const std::vector<std::shared_ptr<nGameObject>> &children) {
	m_RelativePivotChildren.reserve(m_RelativePivotChildren.size() + children.size());
	m_RelativePivotChildren.insert(m_RelativePivotChildren.end(), children.begin(), children.end());
}

//感覺可以優化 在渲染前一次性修改
void Camera::Update() {
	for (const auto &child:m_RelativePivotChildren)
	{
		//變更坐標軸
		// child->SetPivot(m_CameraWorldCoord.translation - child->m_WorldCoord);//成功 - 跟著鏡頭縮放旋轉 但是改變Object Pivot以後槍旋轉點、子彈從槍口發射可能會有問題
		child->m_Transform.translation = (child->m_WorldCoord - m_CameraWorldCoord.translation) * m_CameraWorldCoord.scale;
		child->m_Transform.scale = m_CameraWorldCoord.scale;
		child->m_Transform.rotation = m_CameraWorldCoord.rotation;

	}

}



