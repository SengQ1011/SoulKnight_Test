//
// Created by QuzzS on 2025/2/28.
//

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Observer.hpp"
#include "Util/Transform.hpp"
#include "pch.hpp"

class nGameObject;

class Camera: public InputObserver {
public:
	explicit Camera(const std::vector<std::shared_ptr<nGameObject>> &pivotChildren = {});
	~Camera() override = default;

	void onInputReceived(const std::set<char>& keys) override;

	void MoveCamera(const glm::vec2& displacement);
	void SetFollowTarget(const std::shared_ptr<nGameObject>& target);
	void ZoomCamera(float zoomLevel);
	void RotateCamera(float degree);

	void AddChild(const std::shared_ptr<nGameObject> &child);
	void AddChildren(const std::vector<std::shared_ptr<nGameObject>> &children);
	void RemoveChild(const std::shared_ptr<nGameObject>& child);

	void Update();
	void UpdateZIndex(const std::shared_ptr<nGameObject> &child) const;

	void SetMapSize(const float mapSize) {m_MapYSize = mapSize;}
	[[nodiscard]] float GetMapSize() const { return m_MapYSize;}
	[[nodiscard]] Util::Transform GetCameraWorldCoord() const { return m_CameraWorldCoord;}

protected:
	Util::Transform m_CameraWorldCoord;
	float m_MapYSize = 0.0f; // 用來動態調整ZIndex

private:
	std::weak_ptr<nGameObject> m_FollowTarget;
	std::vector<std::shared_ptr<nGameObject>> m_Children;

	void UpdateChildViewportPosition(const std::shared_ptr<nGameObject> &child);
	bool NotShouldBeVisible(const std::shared_ptr<nGameObject> &child) const;
};

#endif //CAMERA_HPP
