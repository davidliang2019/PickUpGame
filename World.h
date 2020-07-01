//
//	World.h
//

#ifndef WORLD_H
#define WORLD_H

#include <fstream>
#include <vector>
#include "Disk.h"

class World {
public:
	/* constructors and destructor */
	//World();
	//~World() = default;

	/* member functions */
	void init(std::string file_name);	// initial the world class from file
	void draw();					// display all disks
	int getDiskCount() const;
	float getRadius() const;
	int getDiskNumber(ObjLibrary::Vector3 position, float radius) const;
	int getDiskType(ObjLibrary::Vector3 position, float radius) const;
	const Disk& getDisk(int disk_number) const;
	bool isOnDisk(ObjLibrary::Vector3 position, float radius) const;
	float getHeight(ObjLibrary::Vector3 position, float radius) const;
	unsigned int getClosestDiskIndex(const ObjLibrary::Vector3& position) const;
	const Disk& getClosestDisk(const ObjLibrary::Vector3& position) const;
	bool isCylinderOnAnyDisk(const ObjLibrary::Vector3& position, float radius) const;

private: 
	/* member variables */
	float m_radius;
	std::vector<Disk> m_disks;
};

#endif