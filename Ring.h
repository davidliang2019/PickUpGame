//
//	Ring.h
//

#ifndef RING_H
#define RING_H

#include "ObjLibrary/Vector3.h"
#include "World.h"
#include "MovementGraph.h"
#include <stack>

class Ring {
public:
	
	Ring();
	~Ring() = default;
	Ring(MovementGraph graph);

	void draw();
	//void drawLine();
	void drawPath(MovementGraph& graph) const;
	bool update(const World& world, MovementGraph& graph);
	void updatePath(const World& world, MovementGraph& graph);
	void pickingUp(ObjLibrary::Vector3 position, float radius, float half_height);
	bool isPickedup();
	unsigned int getPoint();
	unsigned int getNodeId();
private:
	void loadModel();
	bool isModelsLoaded() const;
	bool isArrived() const;
	void moveTowardsTarget(const World& world);
	void moveInCircle(const World& world, const unsigned int disk_id);

	std::stack<unsigned int> m_path;
	unsigned int start_id;
	unsigned int target_id;
	bool on_same_disk;

	ObjLibrary::Vector3 r_position;
	ObjLibrary::Vector3 target;

	float angle;
	bool pickup;
};

#endif