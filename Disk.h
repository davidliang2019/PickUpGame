//
//	Disk.h
//

#ifndef DISK_H
#define DISK_H

#include "Heightmap.h"
#include "ObjLibrary/Vector3.h"

class Disk {
public:
	/* constructors and destructor */
	Disk();
	Disk(ObjLibrary::Vector3 position, float radius);
	~Disk() = default;

	/* member functions */
	void draw();
	ObjLibrary::Vector3 getPosition() const { return m_position; }
	float getRadius() const { return m_radius; }
	int getDiskType() const { return m_diskType; }
	float getHeight(ObjLibrary::Vector3 position) const;

private:
	int calculateDiskType(float radius);
	/* member variables */
	ObjLibrary::Vector3 m_position;
	float m_radius;
	int m_diskType;
	Heightmap m_heightmap;
};

#endif