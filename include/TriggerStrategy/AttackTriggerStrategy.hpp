//
// Created by QuzzS on 2025/4/29.
//

#ifndef ATTACKTRIGGERSTRATEGY_HPP
#define ATTACKTRIGGERSTRATEGY_HPP

#include "ITriggerStrategy.hpp"

class AttackTriggerStrategy : public ITriggerStrategy {
public:
	int m_Damage;

	explicit AttackTriggerStrategy(const int damage)
		: m_Damage(damage) {}

	void OnTriggerEnter(std::shared_ptr<nGameObject> self,
						std::shared_ptr<nGameObject> other) override;
};

#endif //ATTACKTRIGGERSTRATEGY_HPP
