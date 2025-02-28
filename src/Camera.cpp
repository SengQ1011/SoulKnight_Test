//
// Created by QuzzS on 2025/2/28.
//

#include "Camera.hpp"

Camera::Camera(const std::vector<std::shared_ptr<Util::GameObject>> &children) : m_Children(children)
{
	m_Offset.translation = {0.0f,0.0f};
	m_Offset.rotation = 0.0f;
	m_Offset.scale = {0.0f,0.0f};
}

void Camera::MoveCamera(const glm::vec2 &displacement)
{
	m_Offset.translation = displacement * Util::Time::GetDeltaTimeMs() / 5.0f / std::sqrt(displacement.x * displacement.x + displacement.y * displacement.y);
}

void Camera::ZoomCamera(const float zoomLevel)
{
	m_Offset.scale = glm::vec2(1.0f,1.0f) * zoomLevel;
}

void Camera::RotateCamera(const float degree)
{
	m_Offset.rotation = degree;
}

void Camera::ResetOffset()
{
	m_Offset.translation = {0.0f,0.0f};
	m_Offset.scale = {0.0f,0.0f};
	m_Offset.rotation = 0.0f;
}

void Camera::AddChild(const std::shared_ptr<Util::GameObject> &child) {
	m_Children.push_back(child);
}

void Camera::RemoveChild(std::shared_ptr<Util::GameObject> child) {
	m_Children.erase(std::remove(m_Children.begin(), m_Children.end(), child),
									 m_Children.end());
}

void Camera::AddChildren(
		const std::vector<std::shared_ptr<Util::GameObject>> &children) {
	m_Children.reserve(m_Children.size() + children.size());
	m_Children.insert(m_Children.end(), children.begin(), children.end());
}

//感覺可以優化 在渲染前一次性修改
void Camera::Update() {
	for (const auto &child:m_Children)
	{
		//child->m_Transform.scale = {1.26f, 1.26f}; //熒幕放大
		child->m_Transform.translation -= m_Offset.translation;
		child->m_Transform.scale += m_Offset.scale;
		child->m_Transform.rotation -= m_Offset.rotation;

		//坐標變換？？
		//child->SetPivot(child)
	}
	ResetOffset();
}



