//
// Created by QuzzS on 2025/2/28.
//

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "pch.hpp"
#include "Util/Transform.hpp"
#include "Util/Time.hpp"
#include "Util/GameObject.hpp"

class Camera
{
public:
	explicit Camera(const std::vector<std::shared_ptr<Util::GameObject>> &children = {});
	~Camera() = default;

	void MoveCamera(const glm::vec2& displacement);
	void ZoomCamera(float zoomLevel);
	void RotateCamera(float degree);
	void ResetOffset();

	[[nodiscard]] Util::Transform GetOffset() const { return m_Offset;}

	void AddChild(const std::shared_ptr<Util::GameObject> &child);

	void AddChildren(const std::vector<std::shared_ptr<Util::GameObject>> &children);

	void RemoveChild(std::shared_ptr<Util::GameObject> child);

	void Update();

protected:
	Util::Transform m_Offset;
private:
	std::vector<std::shared_ptr<Util::GameObject>> m_Children;
};

#endif //CAMERA_HPP
