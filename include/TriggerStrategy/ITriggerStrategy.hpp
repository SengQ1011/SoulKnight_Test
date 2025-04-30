//
// Created by QuzzS on 2025/4/29.
//

#ifndef ITRIGGERSTRATEGY_HPP
#define ITRIGGERSTRATEGY_HPP

#include <memory>

class nGameObject;

class ITriggerStrategy {
public:
	virtual ~ITriggerStrategy() = default;
	virtual void OnTriggerEnter(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other) = 0;
	virtual void OnTriggerStay(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other) {};
	virtual void OnTriggerExit(std::shared_ptr<nGameObject> self, std::shared_ptr<nGameObject> other) {};
};

#endif //ITRIGGERSTRATEGY_HPP
