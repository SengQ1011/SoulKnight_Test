//
// Created by tjx20 on 3/25/2025.
//

#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include "ObserveManager/ObserveManager.hpp"
#include "Util/Input.hpp"

class InputManager: public ObserveManager{
public:
	void Update() override;
	//----Getter-----
	std::set<char> GetActiveKey() { return m_activeKeys; }

	//----Setter-----
	void onKeyPressed(char key);
	void onKeyReleased(char key);

	void notifyObserver() override;

private:
	std::set<char> m_activeKeys;  // 記錄目前按下的按鍵
};

#endif //INPUTMANAGER_HPP
