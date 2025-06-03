//
// Created by QuzzS on 2025/5/22.
//

#ifndef UISLIDER_HPP
#define UISLIDER_HPP

#include "Override/nGameObject.hpp"


struct PositionChangedEvent;
class nGameObject;
class UIButton;

class UISlider : public nGameObject
{
public:
	explicit UISlider(const std::function<float()>& listenFunction,
					  const std::shared_ptr<nGameObject>& track,
					  const glm::vec2& borderWidth = glm::vec2(0),
					  bool hasButton = false,
					  const std::string& buttonImage = "",
					  const std::function<void(float)>& trackFunction = nullptr);
	~UISlider() override = default;
	void Start(); //用來建構後設定
	void Update() override;
	void OnEventReceived(const EventInfo &eventInfo) override;

	// void SetFunction(const std::function<void()> &function) {onChange = function;};
	void DrawDebugUI() {};

	void SetTrack(const std::shared_ptr<nGameObject> &track) {m_Track = track;}
	std::shared_ptr<nGameObject> GetTrack() {return m_Track;}

	void SetButton(const std::shared_ptr<UIButton> &button) {m_Button = button;}
	std::shared_ptr<UIButton> GetButton() {return m_Button;}

	void SetTrackFunction(const std::function<void(float)> &function) {m_TrackFunction = function;};
protected:
	void TrackFollowButton(const PositionChangedEvent& eventInfo);
	glm::vec2 m_BorderWidth = glm::vec2(0, 0);
	std::function<float()> m_ListenFunction = nullptr;
	std::function<void(float)> m_TrackFunction = nullptr;
	std::shared_ptr<nGameObject> m_Track = nullptr;
	std::shared_ptr<UIButton> m_Button = nullptr;
	bool isFollowButton = false;
};

#endif //UISLIDER_HPP
