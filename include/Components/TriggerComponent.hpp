//
// Created by QuzzS on 2025/5/1.
//

#ifndef TRIGGERCOMPONENT_HPP
#define TRIGGERCOMPONENT_HPP

#include "Component.hpp"
#include "unordered_set"
class ITriggerStrategy;

class TriggerComponent : public Component {

protected:
	std::unique_ptr<ITriggerStrategy> m_TriggerStrategy = nullptr;
	std::unordered_set<std::shared_ptr<nGameObject>> m_PreviousTriggerTargets;
	std::unordered_set<std::shared_ptr<nGameObject>> m_CurrentTriggerTargets;
};

#endif //TRIGGERCOMPONENT_HPP
