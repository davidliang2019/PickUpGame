//
//	Ring.cpp
//

#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"
#include "GetGlut.h"
#include "DiskType.h"
#include "Ring.h"

using namespace ObjLibrary;
using namespace std;

namespace {
	const float RADIUS = 0.7f;
	const float HALF_HEIGHT = 0.1f;
	const float SPEED = 2.5f / 60.0f;
	const float ROTATION = 75.0f * SPEED;
	const unsigned int POINT_OF_RING = 1;

	DisplayList ring_list;
	bool g_is_initialized = false;
}

Ring::Ring()
	: r_position(Vector3(0.0, 0.0, 0.0))
	, target(Vector3(0.0, 0.0, 0.0))
	, angle(0.0f)
	, pickup(false) 
	, start_id()
	, target_id () 
	, m_path()
{}

Ring::Ring(MovementGraph graph)
	: angle(0.0f)
	, pickup(false) 
{
	m_path = graph.getPath();
	start_id = m_path.top();
	r_position = graph.getPosition(m_path.top());
	m_path.pop();
	target_id = m_path.top();
	target = graph.getPosition(m_path.top());
	m_path.pop();
	on_same_disk = (graph.getDiskId(start_id) == graph.getDiskId(target_id));
}

void Ring::draw()
{
	if (!isModelsLoaded())
		loadModel();

	if (!isPickedup()) {
		// draw this ring
		glPushMatrix();
		glTranslated(r_position.x, r_position.y + HALF_HEIGHT, r_position.z);
		glRotated(angle, 0.0, 1.0, 0.0);
		ring_list.draw();
		glPopMatrix();
	}
}

void Ring::drawPath(MovementGraph& graph) const
{
	const float LINE_ABOVE = 0.5f;

	glLineWidth(3.0);
	glColor3d(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	glVertex3d(r_position.x, r_position.y, r_position.z);
	glVertex3d(r_position.x, r_position.y + LINE_ABOVE, r_position.z);
	glVertex3d(target.x, target.y + LINE_ABOVE, target.z);
	glEnd();
	glLineWidth(1.0);

	stack<unsigned int> copy_path = m_path;
	Vector3 current_node = target;
	while (!copy_path.empty()) {
		Vector3 next_node = graph.getPosition(copy_path.top());
		copy_path.pop();
		glLineWidth(3.0);
		glColor3d(1.0, 1.0, 1.0);
		glBegin(GL_LINE_STRIP);
		glVertex3d(current_node.x, current_node.y + LINE_ABOVE, current_node.z);
		glVertex3d(next_node.x, next_node.y + LINE_ABOVE, next_node.z);
		glEnd();
		glLineWidth(1.0);
		current_node = next_node;
	}
}

bool Ring::update(const World& world, MovementGraph& graph)
{
	if (!isPickedup())
	{
		if (isArrived()) {
			if (m_path.empty())
				return false;
			start_id = target_id;
			target_id = m_path.top();
			target = graph.getPosition(m_path.top());
			m_path.pop();
			on_same_disk = (graph.getDiskId(start_id) == graph.getDiskId(target_id));
		}

		if (on_same_disk)
			moveInCircle(world, graph.getDiskId(start_id));
		else
			moveTowardsTarget(world);

		// maintain disk height
		r_position.y = HALF_HEIGHT + world.getHeight(r_position, RADIUS);
	}
	return true;
}

void Ring::updatePath(const World& world, MovementGraph& graph)
{
	// update path, position, and target position
	m_path = graph.getPath();
	start_id = m_path.top();
	r_position = graph.getPosition(m_path.top());
	m_path.pop();
	target_id = m_path.top();
	target = graph.getPosition(m_path.top());
	m_path.pop();
	on_same_disk = (graph.getDiskId(start_id) == graph.getDiskId(target_id));

	// choose the move methor
	if (on_same_disk)
		moveInCircle(world, graph.getDiskId(start_id));
	else
		moveTowardsTarget(world);

	// maintain disk height
	r_position.y = HALF_HEIGHT + world.getHeight(r_position, RADIUS);
}

void Ring::pickingUp(Vector3 position, float radius, float half_height)
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

bool Ring::isPickedup()
{
	return pickup;
}

unsigned int Ring::getPoint()
{
	return POINT_OF_RING;
}

void Ring::loadModel()
{
	assert(!isModelsLoaded());

	g_is_initialized = true;

	ring_list = ObjModel("Models/ring.obj").getDisplayList();

	assert(isModelsLoaded());
}

bool Ring::isModelsLoaded() const
{
	return g_is_initialized;
}

bool Ring::isArrived() const
{
	float distance_h = (float)r_position.getDistanceXZ(target);
	if (distance_h < RADIUS)
		return TRUE;
	else
		return FALSE;
}

unsigned int Ring::getNodeId()
{
	return target_id;
}

void Ring::moveTowardsTarget(const World& world)
{
	unsigned int disk_type = world.getClosestDisk(r_position).getDiskType();
	float move_distance = SPEED * DiskType::getRingSpeedFactor(disk_type);
	angle += ROTATION * DiskType::getRingSpeedFactor(disk_type);

	Vector3 here_to_target = target - r_position;

	// move straight to target position
	Vector3 forward = here_to_target.getComponentXZ();
	if (!forward.isZero())
		r_position += forward.getCopyWithNorm(move_distance);

	// stop at target position
	if (r_position.isDistanceXZLessThan(target, move_distance))
		r_position = target;
}

void Ring::moveInCircle(const World& world, const unsigned int disk_id)
{
	// calculate the distance for 1 fame
	const unsigned int disk_type = world.getClosestDisk(r_position).getDiskType();
	const float move_distance = SPEED * DiskType::getRingSpeedFactor(disk_type);
	angle += ROTATION * DiskType::getRingSpeedFactor(disk_type);

	// get disk position of center and radius
	const Disk disk = world.getDisk(disk_id);
	const Vector3 disk_center = disk.getPosition();
	const float disk_radius = disk.getRadius();

	//calculate tangent direction
	Vector3 start_to_center = disk_center - r_position;
	start_to_center.normalize();
	const double desired_distance = disk_radius - 0.7; // OR " - RING_RADIUS"

	//apply the law of cosine
	double direction_change = acos(0.5 * move_distance / desired_distance);

	Vector3 chord1 = start_to_center.getRotatedY(direction_change) * move_distance;
	Vector3 chord2 = start_to_center.getRotatedY(-direction_change) * move_distance;
	Vector3 new_position1 = chord1 + r_position;
	Vector3 new_position2 = chord2 + r_position;

	if (target.getDistanceXZ(new_position1) <= target.getDistanceXZ(new_position2))
		r_position = new_position1;
	else
		r_position = new_position2;

	// adjust the ring position to 0.7m inside the edge
	start_to_center = r_position - disk_center;
	start_to_center.normalize();
	r_position = disk_center + start_to_center * desired_distance;
}