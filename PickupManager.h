//
//	PickupManager.h
//

#ifndef PICKUPMANAGER_H
#define PICKUPMANAGER_H

#include <vector>
#include "ObjLibrary/Vector3.h"
#include "Rod.h"
#include "Ring.h"
#include "World.h"

class PickupManager
{
public:
	PickupManager();
	~PickupManager() = default;

	void init(const World& original);
	void draw();
	void update();
	int getScore();
	void pickingUp(ObjLibrary::Vector3 position, float radius, float half_height);
	void drawMovement();
private:
	std::vector <Rod> m_rod;
	std::vector <Ring> m_ring;

	World m_world;
	MovementGraph m_graph;

	unsigned int diskCount;
	unsigned int current_score;
};

#endif
