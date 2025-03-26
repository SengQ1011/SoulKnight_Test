//
// Created by tjx20 on 3/25/2025.
//

#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include "Observer.hpp"
#include "Util/Input.hpp"

class InputManager {
public:

	void listenInput();

	//----Getter-----
	std::vector<std::shared_ptr<Observer>> GetListener() { return m_Observer; }
	std::set<char> GetActiveKey() { return m_activeKeys; }

	//----Setter-----
	void addObserver(const std::shared_ptr<Observer> &observer);
	void removeObserver(const std::shared_ptr<Observer> &observer);
	void onKeyPressed(char key);
	void onKeyReleased(char key);

	void notifyObserver();

private:
	std::vector<std::shared_ptr<Observer>> m_Observer;
	std::set<char> m_activeKeys;  // 記錄目前按下的按鍵
};

#endif //INPUTMANAGER_HPP
