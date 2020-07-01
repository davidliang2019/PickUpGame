//
//	Enemy.h
//

#ifndef ENEMY_H
#define ENEMY_H

//#include "ObjLibrary/Vector3.h"
#include "World.h"
#include "Player.h"
#include "Bat.h"

class Enemy {
public:
	void init(World& g_world);
	void update(Player& player);
	void draw();
	bool isMoreHit();
	ObjLibrary::Vector3 getHitVelocity();

private:
	void updateList(ObjLibrary::Vector3 velocity);

	std::vector<Bat> m_bats;
	std::vector<ObjLibrary::Vector3> velocityList;
	World m_world;
};

#endif
