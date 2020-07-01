//
//	Heightmap.h
//

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <vector>
#include "ObjLibrary/TextureManager.h"
#include "ObjLibrary/DisplayList.h"
#include "ObjLibrary/Vector3.h"
#include "NoiseField.h"

class Heightmap {
public:
	/* constructors and destructor */
	Heightmap(int diskType);
	Heightmap();
	~Heightmap() = default;

	/* member functions */
	void draw();
	float getHeight(float x, float z) const;
private:
	// initialize the heightmap class
	void initHeightmapHeights();
	void initHeightmapDisplayList();
	// support functions to calculate heights for 5 disks type
	void redRockGenerator();
	void leafyGenerator();
	void icyGenerator();
	void sandyGenerator();
	void greyRockGenerator();
	// help function to generate the surface of disks
	void leafyPreprocess(float number[]);
	float leafyHeight(float x, float y, float number[]);
	float icyHeight(float x, float z, ObjLibrary::Vector3 random_points[]);
	float edgeFactor(float  x, float y);
	
	/* member variables */
	int m_diskType;
	int m_heightmap_size;
	int m_repeat;

	int indexing(int i, int j) const
	{
		return j * m_heightmap_size + i;
	}
	std::vector<float> m_heights;
	ObjLibrary::DisplayList heightmap_list;
};

#endif