//
// Created by QuzzS on 2025/2/28.
//

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Util/Transform.hpp"
#include "pch.hpp"
#include "Override/nGameObject.hpp"

class Camera
{
public:
	explicit Camera(const std::vector<std::shared_ptr<nGameObject>> &children = {});
	~Camera() = default;

	void MoveCamera(const glm::vec2& displacement);
	void ZoomCamera(float zoomLevel);
	void RotateCamera(float degree);
	void ResetOffset();

	[[nodiscard]] Util::Transform GetOffset() const { return m_Offset;}
	[[nodiscard]] Util::Transform GetTransform() const { return m_Transform;}


	void AddChild(const std::shared_ptr<nGameObject> &child);

	void AddChildren(const std::vector<std::shared_ptr<nGameObject>> &children);

	void RemoveChild(std::shared_ptr<nGameObject> child);

	void Update();

protected:
	Util::Transform m_Offset;
	Util::Transform m_Transform;
private:
	std::vector<std::shared_ptr<nGameObject>> m_Children;
};

#endif //CAMERA_HPP
