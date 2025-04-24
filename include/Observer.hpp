//
// Created by tjx20 on 3/25/2025.
//

#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <set>
#include <memory>

class Character;
// === 觀察者Interface ===
// 專門處理輸入的觀察者介面
class InputObserver {
public:
	virtual void onInputReceived(const std::set<char>& keys) = 0;
	virtual ~InputObserver() = default;
};

// 專門處理位置更新的觀察者介面
class TrackingObserver {
public:
	virtual ~TrackingObserver() = default;

	// 玩家專用：接收最近的敵人坐標=====FollowerComponent====
	virtual void OnEnemyPositionUpdate(std::weak_ptr<Character> enemy){}
	// 敵人專用：接收玩家坐標
	virtual void OnPlayerPositionUpdate(std::weak_ptr<Character> player){}
	virtual void OnPlayerLost(){}	// 通知失去玩家視野
};
#endif //OBSERVER_HPP
