//
// Created by tjx20 on 3/21/2025.
//

//AnimationComponent.hpp

#ifndef ANIMATIONCOMPONENT_HPP
#define ANIMATIONCOMPONENT_HPP

#include "Components/Component.hpp"
class Animation;


class AnimationComponent : public Component
{
public:
	explicit AnimationComponent(std::unordered_map<State, std::shared_ptr<Animation>> animations);
	~AnimationComponent() override = default;

	void Init() override;
	void Update() override;
	void SetAnimation(State state);
	void SetSkillEffect(bool play, glm::vec2 offset= glm::vec2(0.0f, 0.0f));
	bool IsUsingSkillEffect() const { return m_useSkillEffect; }

	[[nodiscard]] std::shared_ptr<Animation> GetCurrentAnimation() const { return m_currentAnimation; }
	[[nodiscard]] std::shared_ptr<Animation> GetCurrentEffect() const { return m_effectAnimation; }
	[[nodiscard]] std::shared_ptr<Animation> GetAnimation(State state) const {
		auto it = m_Animations.find(state);
		return it != m_Animations.end() ? it->second : nullptr;
	}

private:
	std::unordered_map<State, std::shared_ptr<Animation>> m_Animations; // 狀態對應動畫
	std::shared_ptr<Animation> m_currentAnimation; // 現在的動畫
	std::shared_ptr<Animation> m_effectAnimation; // 現在的特效動畫
	bool m_useSkillEffect = false;
};


#endif //ANIMATIONCOMPONENT_HPP
