//
// Created by tjx20 on 3/25/2025.
//

#include "Components\InputComponent.hpp"

 InputComponent::InputComponent() { LOG_DEBUG("InputComponent Success"); }


void InputComponent::onInputReceived(const std::set<char>& keys) {
 	glm::vec2 movement(0.0f, 0.0f);
	if (keys.count('W')) movement.y += 1.0f;
	if (keys.count('S')) movement.y -= 1.0f;
	if (keys.count('A')) movement.x -= 1.0f;
	if (keys.count('D')) movement.x += 1.0f;

	if (movement.x != 0.0f || movement.y != 0.0f) {
		auto character = GetOwner<Character>();
		if(character) {
			auto movementComponent = character->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
			character->move(movement);
			LOG_DEBUG("Location:{}", movementComponent->GetPosition());
		}
	}

}