//
// Created by tjx20 on 6/1/2025.
//

#ifndef WALLETCOMPONENT_HPP
#define WALLETCOMPONENT_HPP

#include "Components/Component.hpp"

class WalletComponent final : public Component {
public:
	explicit WalletComponent(int startMoney = 0) :
		Component(ComponentType::WALLET), m_Money(startMoney) {}

	void AddMoney(int amount) { m_Money += amount; }
	bool SpendMoney(int amount) {
		if (m_Money >= amount) {
			m_Money -= amount;
			return true;
		}
		return false;
	}
	int GetMoney() const { return m_Money; }
	void SetMoney(int amount) { m_Money = amount; }

private:
	int m_Money = 0;
};


#endif //WALLETCOMPONENT_HPP
