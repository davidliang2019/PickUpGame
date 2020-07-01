//
//	Bat.h
//

#ifndef BAT_H
#define BAT_H

#include "ObjLibrary/Vector3.h"
#include "World.h"
#include "Player.h"

class Bat {
public:
	Bat();
	~Bat() = default;
	Bat(ObjLibrary::Vector3 position);

	void draw();
	void drawLine();
	void update(World& world, Player& player);
	bool isDead() const;
	ObjLibrary::Vector3 getHitVelocity();

private:
	void loadModel();
	bool isModelsLoaded() const;
	bool isArrived() const;
	void pursue(Player& player);
	void explore(World& world);
	void handleCollisions(ObjLibrary::Vector3& target);
	void Collide();
	
	ObjLibrary::Vector3 m_position;
	ObjLibrary::Vector3 m_forward;
	ObjLibrary::Vector3 m_velocity;
	ObjLibrary::Vector3 m_acceleration;
	ObjLibrary::Vector3 target_position;
	ObjLibrary::Vector3 hit_velocity;

	bool is_dead;
};

#endif