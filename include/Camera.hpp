//
// Created by QuzzS on 2025/2/28.
//

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <random>
#include <vector>
#include "Observer.hpp"
#include "Util/Timer.hpp"
#include "Util/Transform.hpp"


class nGameObject;

class Camera : public InputObserver
{
public:
	explicit Camera(const std::vector<std::shared_ptr<nGameObject>> &pivotChildren = {});
	~Camera() override;

	void onInputReceived(const std::set<char> &keys) override;

	void MoveCamera(const glm::vec2 &displacement);
	void SetFollowTarget(const std::shared_ptr<nGameObject> &target);
	void ZoomCamera(float zoomLevel);
	void RotateCamera(float degree);

	// 抖動特效
	void StartShake(float duration = 0.3f, float intensity = 10.0f);
	void StopShake();
	bool IsShaking() const;

	// 事件系統初始化
	void InitializeEventListeners();

	// 初始化/結束階段使用
	void AddChild(const std::shared_ptr<nGameObject> &child);
	void AddChildren(const std::vector<std::shared_ptr<nGameObject>> &children);
	void RemoveChild(const std::shared_ptr<nGameObject> &child);
	void MarkForRemoval(const std::shared_ptr<nGameObject> &child);
	bool FindChild(const std::shared_ptr<nGameObject> &child);
	// 更新階段使用
	void SafeAddChild(const std::shared_ptr<nGameObject> &child) { m_ToAddList.push_back(child); }
	void SafeRemoveChild(const std::shared_ptr<nGameObject> &child) { m_ToRemoveList.push_back(child); }

	void Update();
	void UpdateZIndex(const std::shared_ptr<nGameObject> &child) const;

	void SetMapSize(const float mapSize) { m_MapYSize = mapSize; }
	[[nodiscard]] float GetMapSize() const { return m_MapYSize; }
	[[nodiscard]] Util::Transform GetCameraWorldCoord() const { return m_CameraWorldCoord; }

protected:
	Util::Transform m_CameraWorldCoord;
	float m_MapYSize = 0.0f; // 用來動態調整ZIndex

private:
	std::weak_ptr<nGameObject> m_FollowTarget;
	std::vector<std::shared_ptr<nGameObject>> m_Children;
	std::vector<std::weak_ptr<nGameObject>> m_ToAddList;
	std::vector<std::weak_ptr<nGameObject>> m_ToRemoveList;

	// 抖動相關
	Util::Timer m_ShakeTimer;
	glm::vec2 m_OriginalPosition{0.0f};
	glm::vec2 m_ShakeOffset{0.0f};
	float m_ShakeIntensity = 0.0f;
	bool m_IsShaking = false;
	std::mt19937 m_RandomGenerator;
	std::uniform_real_distribution<float> m_RandomDistribution{-1.0f, 1.0f};

	void UpdateChildViewportPosition(const std::shared_ptr<nGameObject> &child);
	bool NotShouldBeVisible(const std::shared_ptr<nGameObject> &child) const;
};

#endif // CAMERA_HPP
