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
#include "Observer.hpp"

class Camera: public Observer {
public:
	explicit Camera(const std::vector<std::shared_ptr<nGameObject>> &pivotChildren = {});
	~Camera() = default;

	void onInputReceived(const std::set<char>& keys) override;

	void MoveCamera(const glm::vec2& displacement);
	void SetFollowTarget(const std::shared_ptr<nGameObject>& target);
	void ZoomCamera(float zoomLevel);
	void RotateCamera(float degree);

	void AddRelativePivotChild(const std::shared_ptr<nGameObject> &child);
	void AddRelativePivotChildren(const std::vector<std::shared_ptr<nGameObject>> &children);
	void RemoveRelativePivotChild(const std::shared_ptr<nGameObject>& child);

	void Update();
	void UpdateZIndex(std::shared_ptr<nGameObject> child);

	void SetMapSize(const float mapSize) {m_MapSize = mapSize;}
	[[nodiscard]] float GetMapSize() const { return m_MapSize;}
	[[nodiscard]] Util::Transform GetCameraWorldCoord() const { return m_CameraWorldCoord;}

protected:
	Util::Transform m_CameraWorldCoord;
	Beacon m_Beacon;
	float m_MapSize = 0.0f;
private:
	std::weak_ptr<nGameObject> m_FollowTarget;
	std::vector<std::shared_ptr<nGameObject>> m_RelativePivotChildren;
};

#endif //CAMERA_HPP
