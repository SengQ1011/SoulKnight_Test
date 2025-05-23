//
// Created by QuzzS on 2025/4/29.
//

#ifndef ATTACKTRIGGERSTRATEGY_HPP
#define ATTACKTRIGGERSTRATEGY_HPP

#include "ITriggerStrategy.hpp"

class AttackTriggerStrategy : public ITriggerStrategy {
public:
	int m_Damage;
	StatusEffect m_elementalDamage;

	explicit AttackTriggerStrategy(const int damage, const StatusEffect &elementalDamage)
		: m_Damage(damage), m_elementalDamage(elementalDamage) {}

	void OnTriggerEnter(std::shared_ptr<nGameObject> self,
						std::shared_ptr<nGameObject> other) override;
};

#endif //ATTACKTRIGGERSTRATEGY_HPP
