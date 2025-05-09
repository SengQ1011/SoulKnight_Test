//
// Created by QuzzS on 2025/5/1.
//

#ifndef TRIGGERCOMPONENT_HPP
#define TRIGGERCOMPONENT_HPP

#include "Component.hpp"
#include "unordered_set"

class AreaShape;
class ITriggerStrategy;

class TriggerComponent : public Component {

protected:
	std::shared_ptr<AreaShape> m_Area = nullptr;
	std::unique_ptr<ITriggerStrategy> m_TriggerStrategy = nullptr;
	std::unordered_set<std::shared_ptr<nGameObject>> m_PreviousTriggerTargets;
	std::unordered_set<std::shared_ptr<nGameObject>> m_CurrentTriggerTargets;
};

#endif //TRIGGERCOMPONENT_HPP
