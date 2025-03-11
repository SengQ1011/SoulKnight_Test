//
// Created by tjx20 on 3/7/2025.
//

#ifndef HITBOX_HPP
#define HITBOX_HPP

class HitBox {
public:
	HitBox(float x, float y, float width, float height);
	~HitBox() = default;
	float x, y, width, height;

	bool CheckCollision(const HitBox& other) const;
};

#endif //HITBOX_HPP
