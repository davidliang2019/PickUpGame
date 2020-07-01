//
//	World.cpp
//

#include "World.h"
#include "DiskType.h"

using namespace std;
using namespace ObjLibrary;

//World::World() 
	//: m_radius(0.0f) 
	//, m_disks() {}

// this function inputs a path of a file and stores information
// (x, z, radius) for all disks and world radius
void World::init(string file_name)
{
	ifstream in_data;	
	in_data.open(file_name.c_str());
	if (!in_data)
	{
		// can't open file
		exit(1);
	}

	string firstline;	// get rid of the first line
	getline(in_data, firstline);
	if (firstline != "DISK version 1")
	{
		cerr << "Error in loadDisks: Invalid first line \"" << firstline << "\"" << endl;
		exit(1);
	}

	in_data >> m_radius;	//get the world radius
	if (m_radius <= 0.0f)
	{
		cerr << "Error in loadDisks: Non-positive world radius" << endl;
		exit(1);
	}

	int disk_count;
	in_data >> disk_count;		//get the number of disk
	if (disk_count < 0)
	{
		cerr << "Error in loadDisks: Negative disk count" << endl;
		exit(1);
	}

	for (int i = 0; i < disk_count; i++)
	{
		float x, z, radius;
		in_data >> x >> z >> radius;
		m_disks.push_back({ Vector3(x, 0.0, z), radius });
	}
}

// this function displays all the disk from display list to screen
void World::draw()
{
	for (unsigned int i = 0; i < m_disks.size(); i++)
		m_disks[i].draw();
}

int World::getDiskCount() const
{
	return m_disks.size();
}

float World::getRadius() const
{
	//assert(m_radius == NULL || m_radius <= 0.0f);
	return m_radius;
}

// add new member functions for assignment 3
int World::getDiskNumber(ObjLibrary::Vector3 position, float radius) const
{
	float min_dist = m_radius;
	int min_i = -1;
	for (int i = 0; i < (int)m_disks.size(); i++) {
		Vector3 d_position = m_disks[i].getPosition();
		float d_radius = m_disks[i].getRadius();
		float h_distance = (float)d_position.getDistanceXZ(position);
		if (min_dist > h_distance && h_distance < d_radius + radius) {
			min_dist = h_distance;
			min_i = i;
		}
	}
	return min_i;
}

bool World::isOnDisk(ObjLibrary::Vector3 position, float radius) const
{
	int index = getDiskNumber(position, radius);

	if (index < 0)
		return false;
	else
		return true;
}

int World::getDiskType(ObjLibrary::Vector3 position, float radius) const
{
	int index = getDiskNumber(position, radius);

	if (index < 0)
		return 99;
	else
		return m_disks[index].getDiskType();
}

const Disk& World::getDisk(int disk_number) const
{
	return m_disks[disk_number];
}


float World::getHeight(Vector3 position, float radius) const
{
	int index = getDiskNumber(position, radius);

	if (index < 0)
		return 0.0f;
	else
		return m_disks[index].getHeight(position);

}

unsigned int World::getClosestDiskIndex(const Vector3& position) const
{
	int best_disk = 0;
	double best_distance = position.getDistanceXZ(m_disks[0].getPosition()) - m_disks[0].getRadius();

	for (int i = 1; i < (int)m_disks.size(); i++){
		double distance = position.getDistanceXZ(m_disks[i].getPosition()) - m_disks[i].getRadius();
		if (distance < best_distance){
			best_disk = i;
			best_distance = distance;
		}
	}

	return best_disk;
}

const Disk& World::getClosestDisk(const Vector3& position) const
{
	return m_disks[getClosestDiskIndex(position)];
}

bool World::isCylinderOnAnyDisk(const Vector3& position, float radius) const
{
	const Disk& closest_disk = getClosestDisk(position);
	return position.getDistanceXZ(closest_disk.getPosition()) <= radius + closest_disk.getRadius();
}