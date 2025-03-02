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

void Camera::MoveCamera(const glm::vec2 &displacement)
{
	//加位移變化量
	m_CameraWorldCoord.translation += displacement * Util::Time::GetDeltaTimeMs() / 5.0f / std::sqrt(displacement.x * displacement.x + displacement.y * displacement.y);
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
		child->SetPivot(m_CameraWorldCoord.translation - child->m_WorldCoord);//成功 - 跟著鏡頭縮放旋轉
		child->m_Transform.scale = m_CameraWorldCoord.scale;
		child->m_Transform.rotation = m_CameraWorldCoord.rotation;
	}

}



