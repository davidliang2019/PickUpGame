//
//	Disk.cpp
//

#include "Disk.h"
#include "DiskType.h"
#include "ObjLibrary/ObjModel.h"

using namespace std;
using namespace ObjLibrary;
namespace {
	bool is_initialized = false;
	ObjModel disk_model[DiskType::COUNT];
	DisplayList disk_list[DiskType::COUNT];
}

Disk::Disk() : m_position() {}

Disk::Disk(Vector3 position, float radius) 
	: m_position(position)
	, m_radius(radius)
	, m_diskType(calculateDiskType(radius))
	, m_heightmap(m_diskType)
{}

void Disk::draw()
{
	if (!is_initialized) {
		// sign all disk objects to the display list
		for (int i = 0; i < DiskType::COUNT; i++) {
			disk_model[i].load(DiskType::getModelName(i));
			disk_list[i] = disk_model[i].getDisplayList();
		}
		is_initialized = true;
	}

	glPushMatrix();
	glTranslatef((GLfloat)m_position.x, 0.0f, (GLfloat)m_position.z);
	glScalef(m_radius, 1.0f, m_radius);
	disk_list[m_diskType].draw();
	glPopMatrix();

	float length = m_radius * (float)sqrt(2);
	float scaling = length / DiskType::getSideLength(m_diskType);

	glPushMatrix();
	glTranslated(length*(-0.5) + m_position.x, 0.0, length*(-0.5) + m_position.z);
	glScalef(scaling, 1.0, scaling);
	m_heightmap.draw();
	glPopMatrix();
}

int Disk::calculateDiskType(float radius)
{
	if (m_radius < 8.0f)
		return DiskType::RED_ROCK;
	else if (m_radius < 12.0f)
		return DiskType::LEAFY;
	else if (m_radius < 20.0f)
		return DiskType::ICY;
	else if (m_radius < 30.0f)
		return DiskType::SANDY;
	else if (m_radius < 50.0f)
		return DiskType::GREY_ROCK;
	else
		exit(1);
}

// add the new function for Assignment 3
float Disk::getHeight(Vector3 position) const
{
	// follow the hint from Transformations.pdf
	Vector3 p0 = position - m_position;
	Vector3 pd = p0 / m_radius;

	float SQRT2 = (float)sqrt(2);
	float HALF_SQRT2 = 0.5f * SQRT2;
	if (abs(pd.x) >= HALF_SQRT2 || abs(pd.z) >= HALF_SQRT2)
		return 0.0f;
	else {
		Vector3 p1 = pd + Vector3(HALF_SQRT2, 0.0, HALF_SQRT2);
		Vector3 p2 = p1 / SQRT2;
		Vector3 ph = p2 * DiskType::getSideLength(m_diskType);
		return m_heightmap.getHeight((float)ph.x, (float)ph.z);
	}
}