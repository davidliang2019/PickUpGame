//
//	Player.h
//

#ifndef PLAYER_H
#define PLAYER_H

#include "ObjLibrary/Vector3.h"
#include "World.h"
#include "Disk.h"

class Player {
public:
	static const float HALF_HEIGHT;
	static const float RADIUS;
	static const float HALF_PI;
	static const float TWO_PI;

	void init(const World& world);
	void update(const World& world);
	void updateY(const World& world);
	void draw();
	void jump();
	void isAttacked(ObjLibrary::Vector3 velocity);

	void moveForward(const World& world);
	void moveBackward(const World& world);
	void moveLeft(const World& world);
	void moveRight(const World& world);
	void turnLeft();
	void turnRight();

	ObjLibrary::Vector3 getPosition();
	ObjLibrary::Vector3 getForward();
	ObjLibrary::Vector3 getVelocity();
private:
	void loadModel();
	bool isModelsLoaded() const;
	void slidingAcceleration(const Disk& player_disk);

	ObjLibrary::Vector3 m_position;
	ObjLibrary::Vector3 m_forward;
	ObjLibrary::Vector3 m_velocity; 
	ObjLibrary::Vector3 m_acceleration;

	bool jumping;
};

#endif
