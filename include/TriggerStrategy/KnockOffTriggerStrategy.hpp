//
// Created by tjx20 on 5/15/2025.
//

#ifndef KNOCKOFFTRIGGERSTRATEGY_HPP
#define KNOCKOFFTRIGGERSTRATEGY_HPP

#include "ITriggerStrategy.hpp"

class KnockOffTriggerStrategy : public ITriggerStrategy {
public:
	float m_impulseForce;

	explicit KnockOffTriggerStrategy(float impulseForce)
		: m_impulseForce(impulseForce) {}
	~KnockOffTriggerStrategy() override = default;

	// 當 shockwave (self) 跟 other 發生 trigger 進入時被呼叫
	void OnTriggerEnter(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other) override;
};

#endif //KNOCKOFFTRIGGERSTRATEGY_HPP
