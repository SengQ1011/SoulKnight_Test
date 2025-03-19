//
// Created by QuzzS on 2025/2/28.
//

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Beacon.hpp"
#include "Override/nGameObject.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Time.hpp"
#include "Util/Transform.hpp"
#include "pch.hpp"

class Camera
{
public:
	explicit Camera(const std::vector<std::shared_ptr<nGameObject>> &pivotChildren = {});
	~Camera() = default;

	void MoveCamera(const glm::vec2& displacement);
	void CameraFollowWith(const glm::vec2& target);
	void ZoomCamera(float zoomLevel);
	void RotateCamera(float degree);

	[[nodiscard]] Util::Transform GetCameraWorldCoord() const { return m_CameraWorldCoord;}

	void AddRelativePivotChild(const std::shared_ptr<nGameObject> &child);
	void AddRelativePivotChildren(const std::vector<std::shared_ptr<nGameObject>> &children);
	void RemoveRelativePivotChild(const std::shared_ptr<nGameObject>& child);

	void Update();

protected:
	Util::Transform m_CameraWorldCoord;
	Beacon m_Beacon;
private:
	std::vector<std::shared_ptr<nGameObject>> m_RelativePivotChildren;
};

#endif //CAMERA_HPP
