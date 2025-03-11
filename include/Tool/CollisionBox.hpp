//
// Created by tjx20 on 3/7/2025.
//

#ifndef COLLISIONBOX_HPP
#define COLLISIONBOX_HPP

class CollisionBox {
public:
	CollisionBox(float x, float y, float width, float height);
	~CollisionBox() = default;

	bool CheckCollision(const CollisionBox& other) const;

private:
	float x, y, width, height;
};


#endif //COLLISIONBOX_HPP
