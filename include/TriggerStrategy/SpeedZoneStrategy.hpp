//
// Created by tjx20 on 6/12/2025.
//

#ifndef SPEEDZONESTRATEGY_HPP
#define SPEEDZONESTRATEGY_HPP

#include "TriggerStrategy/ITriggerStrategy.hpp"

class SpeedZoneStrategy : public ITriggerStrategy {
public:
	explicit SpeedZoneStrategy(float speedMultiplier, float duration)
		: m_speedRatio(speedMultiplier), m_durationTime(duration) {}

	void OnTriggerEnter(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other) override;
	void OnTriggerStay(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other) override;
	void OnTriggerExit(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other) override {}

private:
	float m_speedRatio;
	float m_durationTime;
};

#endif //SPEEDZONESTRATEGY_HPP
