//
// Created by tjx20 on 6/12/2025.
//

#ifndef SPIKECOMPONENT_HPP
#define SPIKECOMPONENT_HPP

#include "Components/Component.hpp"
#include "Util/Timer.hpp"
#include "TriggerStrategy/AttackTriggerStrategy.hpp"

namespace Core
{
	class Drawable;
}
enum class spikeState
{
	HIDDEN,
	ACTIVE
};

class SpikeComponent final : public Component {
public:
	explicit SpikeComponent(std::vector<std::string> imagePaths, const int damage);
	~SpikeComponent() override= default;

	void Init() override;
	void Update() override;

	/* ---Getter--- */
	[[nodiscard]] spikeState GetCurrentState() const { return m_spikeState; }

	/* ---Setter--- */
	void SetSpikeState(spikeState spikeState) { m_spikeState = spikeState; }

private:
	spikeState m_spikeState = spikeState::HIDDEN;
	std::vector<std::string> m_imagePaths;
	std::vector<std::shared_ptr<Core::Drawable>> m_drawables;
	int m_damage = 0;

	Util::Timer m_activeTimer{2.0f}; // 地刺弹出持续2秒
	Util::Timer m_hiddenTimer{4.0f}; // 地刺收回持续3秒
};

#endif //SPIKECOMPONENT_HPP
