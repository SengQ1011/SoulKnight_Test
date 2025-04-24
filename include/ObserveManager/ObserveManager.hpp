//
// Created by tjx20 on 4/5/2025.
//

#ifndef OBSERVEMANAGER_HPP
#define OBSERVEMANAGER_HPP

#include "Util/Logger.hpp"

class InputObserver;

class ObserveManager {
public:
	virtual ~ObserveManager() = default;
	virtual void Update() = 0;

	//----Getter-----
	std::vector<std::shared_ptr<InputObserver>> GetListener() { return m_Observer; }

	//----Setter-----
	void addObserver(const std::shared_ptr<InputObserver> &observer) {
		if (observer) {
			m_Observer.push_back(observer);
			LOG_DEBUG("Add New Observer");
		}else {
			LOG_ERROR("Failed to Add New Observer");
		}
	}
	void removeObserver(const std::shared_ptr<InputObserver> &observer) {
		// vector/list/array的從begin到end的搜索法：std::find(vec.begin(), vec.end, XXX)
		auto it = std::find(m_Observer.begin(), m_Observer.end(), observer);
		if (it != m_Observer.end()) {
			m_Observer.erase(it);
		}
	}

	virtual void notifyObserver() = 0;

protected:
	std::vector<std::shared_ptr<InputObserver>> m_Observer;
};
#endif //OBSERVEMANAGER_HPP
