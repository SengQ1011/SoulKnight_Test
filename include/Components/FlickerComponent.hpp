//
// Created by Assistant on 2025/1/20.
//

#ifndef FLICKERCOMPONENT_HPP
#define FLICKERCOMPONENT_HPP

#include "Components/Component.hpp"
#include "Util/Timer.hpp"

class FlickerComponent : public Component
{
public:
	explicit FlickerComponent();
	~FlickerComponent() override = default;

	void Update() override;
	void HandleEvent(const EventInfo &eventInfo) override;
	std::vector<EventType> SubscribedEventTypes() const override;

	void StartFlicker(float duration = 0.5f, float interval = 0.05f);
	void StopFlicker();
	bool IsFlickering() const { return m_IsFlickering; }

private:
	Util::Timer m_FlickerTimer;
	bool m_IsFlickering = false;
	bool m_OriginalVisibility = true;
	float m_FlickerInterval = 0.05f;
};

#endif // FLICKERCOMPONENT_HPP
