//
// Created by tjx20 on 5/9/2025.
//

#ifndef REFLECTTRIGGERSTRATEGY_HPP
#define REFLECTTRIGGERSTRATEGY_HPP

#include "ITriggerStrategy.hpp"

class ReflectTriggerStrategy : public ITriggerStrategy {
public:
	ReflectTriggerStrategy() = default;
	~ReflectTriggerStrategy() override = default;

	// 當 slash (self) 跟 other 發生 trigger 進入時被呼叫
	void OnTriggerEnter(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other) override;
};

#endif //REFLECTTRIGGERSTRATEGY_HPP
