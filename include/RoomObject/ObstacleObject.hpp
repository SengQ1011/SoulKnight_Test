//
// Created by QuzzS on 2025/3/8.
//

#ifndef OBSTACLEOBJECT_HPP
#define OBSTACLEOBJECT_HPP
#include "RoomObject.hpp"
#include "json.hpp"

using json = nlohmann::json;

class ObstacleObject final : public RoomObject
{
public:
	ObstacleObject() = default;
	~ObstacleObject() override = default;

	void Start() override;
	void Update() override;
protected:
};

#endif //OBSTACLEOBJECT_HPP
