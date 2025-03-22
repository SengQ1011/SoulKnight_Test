//
// Created by QuzzS on 2025/3/21.
//

//RoomFactory.hpp

#ifndef ROOMFACTORY_HPP
#define ROOMFACTORY_HPP

#include "Factory.hpp"

class RoomFactory : public Factory {
public:
	RoomFactory() = default;
	~RoomFactory() override = default;

	void SetJSON(const std::string& fileName) {m_JSONName = fileName;}

	std::shared_ptr<RoomFactory> createRoom();

protected:
	std::string m_JSONName;

};

#endif //ROOMFACTORY_HPP
