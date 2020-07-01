//
//	Rod.cpp
//

#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"
#include "GetGlut.h"
#include "DiskType.h"
#include "Rod.h"

using namespace ObjLibrary;

namespace {
	const float RADIUS = 0.1f;
	const float HALF_HEIGHT = 1.0f;

	DisplayList rod_list;
	bool g_is_initialized = false;
}

Rod::Rod() 
	: r_position(Vector3(0.0, 0.0, 0.0))
	, pickup(false) 
	, point(1) {}

Rod::Rod(Vector3 position, int disk_type)
	: r_position(position)
	, pickup(false) 
{
	if (disk_type < DiskType::COUNT)
		point = DiskType::getRodPoints(disk_type);
}

void Rod::draw()
{
	if (!isModelsLoaded())
		loadModel();

	if (!isPickedup()) {
		glPushMatrix();
		glTranslated(r_position.x, 
			r_position.y + HALF_HEIGHT, 
			r_position.z);
		rod_list.draw();
		glPopMatrix();
	}
}

void Rod::pickingUp(Vector3 position, float radius, float half_height)
{
	bool collide_height = false;
	bool collide_horizon = false;

	// check horizontal overlap
	float h_distance = (float)r_position.getDistanceXZ(position);
	if (h_distance < radius + RADIUS)
		collide_horizon = true;

	// check vertical overlap
	float v_distance = (float)abs(r_position.y - position.y);
	if (v_distance < half_height + HALF_HEIGHT)
		collide_height = true;

	// combine both horizontal and vertical result
	if (collide_height && collide_horizon)
		pickup = true;
}

bool Rod::isPickedup()
{
	return pickup;
}

unsigned int Rod::getPoint()
{
	return point;
}

bool Rod::isModelsLoaded()
{
	return g_is_initialized;
}

void Rod::loadModel()
{
	assert(!isModelsLoaded());

	g_is_initialized = true;

	rod_list = ObjModel("Models/rod.obj").getDisplayList();

	assert(isModelsLoaded());
}