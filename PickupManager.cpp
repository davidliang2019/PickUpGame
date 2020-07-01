//
//	PickupManager.cpp
//

#include "PickupManager.h"
#include "DiskType.h"
#include "ObjLibrary/SpriteFont.h"
#include "MovementGraph.h"

using namespace std;
using namespace ObjLibrary;

PickupManager::PickupManager()
	: m_world()
	, diskCount(0)
	, current_score(0)
{
	m_rod.clear();
	m_ring.clear();
}

void PickupManager::init(const World& original)
{
	diskCount = original.getDiskCount();
	m_graph.init(original);
	
	for (int i = 0; i < original.getDiskCount(); i++) {
		Vector3 position = original.getDisk(i).getPosition();
		int disk_type = original.getDisk(i).getDiskType();
		float radius = original.getDisk(i).getRadius();
		float height = original.getHeight(position, radius);
		position += Vector3(0.0, height, 0.0);

		m_rod.push_back({ position, disk_type });

		m_graph.displayWhen(i == 0);
		m_graph.pathFinding();
		m_ring.push_back(m_graph);
	}
	m_world = original;
}

void PickupManager::draw()
{
	for (unsigned int i = 0; i < diskCount; i++) {
		m_rod[i].draw();
		m_ring[i].draw();
	}

	// draw white line for ring 0
	m_ring[0].drawPath(m_graph);
}

void PickupManager::update()
{
	for (unsigned int i = 0; i < diskCount; i++) {
		if (!m_ring[i].isPickedup()) {
			bool update_succeed = m_ring[i].update(m_world, m_graph);
			if (!update_succeed) {
				unsigned int node_id = m_ring[i].getNodeId();
				m_graph.displayWhen(i == 0);
				m_graph.updatePath(node_id);
				m_ring[i].updatePath(m_world, m_graph);
			}
		}
	}
}

int PickupManager::getScore()
{
	return current_score;
}

void PickupManager::pickingUp(ObjLibrary::Vector3 position, float radius, float half_height)
{
	unsigned int point = 0;
	unsigned int disk_type = m_world.getDiskType(position, radius);
	for (unsigned int i = 0; i < diskCount; i++) {
		m_rod[i].pickingUp(position, radius, half_height);
		if (m_rod[i].isPickedup())
			point += m_rod[i].getPoint();

		m_ring[i].pickingUp(position, radius, half_height);
		if (m_ring[i].isPickedup())
			point += m_ring[i].getPoint();
	}
	if (current_score < point)
		current_score = point;
}

void PickupManager::drawMovement()
{
	m_graph.drawPath();
	m_graph.drawSphere();
}