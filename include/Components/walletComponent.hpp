//
// Created by tjx20 on 6/1/2025.
//

#ifndef WALLETCOMPONENT_HPP
#define WALLETCOMPONENT_HPP

#include <imgui.h>
#include "Components/Component.hpp"


class walletComponent final : public Component
{
public:
	explicit walletComponent(int startMoney = 0) : Component(ComponentType::WALLET), m_Money(startMoney) {}
	void Update() override
	{
		DrawDebugUI();
		if (m_Money >= MAX_LIMIT)
		{
			m_Money = MAX_LIMIT;
		}
	}

	void AddMoney(int amount) { m_Money += amount; }
	bool SpendMoney(int amount)
	{
		if (m_Money >= amount)
		{
			m_Money -= amount;
			return true;
		}
		return false;
	}
	int GetMoney() const { return m_Money; }
	void SetMoney(int amount) { m_Money = amount; }

	// Debug UI interface
	void DrawDebugUI()
	{
		if (ImGui::CollapsingHeader("Wallet Component Debug"))
		{
			// Display current money amount
			ImGui::Text("Current Money: %d", m_Money);

			// Money adjustment slider
			int tempMoney = m_Money;
			if (ImGui::SliderInt("Set Money", &tempMoney, 0, 9999))
			{
				m_Money = tempMoney;
			}

			// Quick adjustment buttons
			if (ImGui::Button("+ 100"))
			{
				AddMoney(100);
			}
			ImGui::SameLine();
			if (ImGui::Button("+ 500"))
			{
				AddMoney(500);
			}
			ImGui::SameLine();
			if (ImGui::Button("+ 1000"))
			{
				AddMoney(1000);
			}

			// Second row buttons
			if (ImGui::Button("Reset"))
			{
				SetMoney(0);
			}
			ImGui::SameLine();
			if (ImGui::Button("Max Value"))
			{
				SetMoney(9999);
			}

			// Custom amount addition
			static int addAmount = 50;
			ImGui::InputInt("Custom Amount", &addAmount);
			if (ImGui::Button("Add Custom Amount"))
			{
				AddMoney(addAmount);
			}
		}
	}

private:
	int m_Money = 0;
	static constexpr int MAX_LIMIT = 9999;
};


#endif // WALLETCOMPONENT_HPP
