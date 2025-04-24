//
// Created by QuzzS on 2025/4/25.
//

#ifndef IMANAGER_HPP
#define IMANAGER_HPP

class IManager {
public:
	virtual void Update() = 0;
	virtual ~IManager() = default;
};

#endif //IMANAGER_HPP
