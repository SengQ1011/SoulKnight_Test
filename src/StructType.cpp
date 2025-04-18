//
// Created by tjx20 on 4/18/2025.
//
#include "StructType.hpp"
#include "Components/AiComponent.hpp"

std::shared_ptr<AIComponent> EnemyContext::GetAIComp() const {
	return enemy->GetComponent<AIComponent>(ComponentType::AI);
}
