//
//	Bat.cpp
//

#include "Bat.h"
#include "DeltaTime.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"

using namespace std;
using namespace ObjLibrary;
namespace {
	const float HALF_HEIGHT = 0.05f;
	const float RADIUS = 0.5f;
	const float MAX_SPEED = 5.0f;
	const float MAX_ACCELERATION = 8.0f;
	const float EXPLORE_HEIGHT = 15.0f;

	DisplayList bat_list;
	bool g_is_initialized = false;
	unsigned int not_hit_count = 0;
}

Bat::Bat()
	: m_position(Vector3(0.0, EXPLORE_HEIGHT, 0.0))
	, m_forward(-m_position.getNormalized())
	, m_velocity(Vector3(0.0, 0.0, 0.0))
	, m_acceleration(Vector3(0.0, 0.0, 0.0))
	, target_position(m_position)
	, hit_velocity(Vector3(0.0, 0.0, 0.0))
	, is_dead(false)
{}

Bat::Bat(Vector3 position)
	: m_position(position)
	, m_forward(-m_position.getNormalized())
	, m_velocity(Vector3(0.0, 0.0, 0.0))
	, m_acceleration(Vector3(0.0, 0.0, 0.0))
	, target_position(m_position)
	, hit_velocity(Vector3(0.0, 0.0, 0.0))
	, is_dead(false)
{}

void Bat::draw()
{
	if (!isModelsLoaded())
		loadModel();

	glPushMatrix();
	glTranslated(m_position.x, m_position.y + HALF_HEIGHT, m_position.z);
	glRotated(m_forward.getRotationY(), 0.0, 1.0, 0.0);
	bat_list.draw();
	glPopMatrix();

}

void Bat::drawLine()
{
	// draw white line from current position to target position
	glBegin(GL_LINES);
	glColor3d(1.0, 1.0, 1.0);
	glVertex3d(m_position.x, m_position.y, m_position.z);
	glVertex3d(target_position.x, target_position.y, target_position.z);
	glEnd();
}

void Bat::update(World & world, Player& player)
{
	if (isDead()) {
		float height = world.getHeight(m_position, RADIUS);
		if (m_position.y > height) {
			m_position += m_velocity * DeltaTime::FIXED_DELTA_TIME;
			m_velocity += Vector3(0.0, -9.8, 0.0) * DeltaTime::FIXED_DELTA_TIME;
		}
		else
			m_position.y = height;
		return;
	}
	
	Vector3 target = player.getPosition();
	if (m_position.getDistanceXZ(target) <= 20.0)
		pursue(player);
	else
		explore(world);

	Vector3 direction = target_position - m_position;
	m_acceleration = direction.getNormalized() * MAX_ACCELERATION;

	m_position += m_velocity * DeltaTime::FIXED_DELTA_TIME;
	float height = world.getHeight(m_position, RADIUS);
	if (m_position.y <= height) {
		m_position.y = height;
		m_acceleration.y = -m_acceleration.y;
	}

	m_velocity += m_acceleration * DeltaTime::FIXED_DELTA_TIME;
	if (m_velocity.getNorm() > MAX_SPEED)
		m_velocity = m_velocity.getNormalized() * MAX_SPEED;

	if (not_hit_count == 0)
		handleCollisions(target);
	else
		not_hit_count--;
}

bool Bat::isDead() const
{
	return is_dead;
}

ObjLibrary::Vector3 Bat::getHitVelocity()
{
	return hit_velocity;
}

void Bat::loadModel()
{
	assert(!isModelsLoaded());
	g_is_initialized = true;
	bat_list = ObjModel("Models/BrownBat.obj").getDisplayList();
	assert(isModelsLoaded());
}

bool Bat::isModelsLoaded() const
{
	return g_is_initialized;
}

bool Bat::isArrived() const
{
	float distance_h = (float)m_position.getDistanceXZ(target_position);
	if (distance_h <= 2.0f)
		return TRUE;
	else
		return FALSE;
}

void Bat::pursue(Player & player)
{
	target_position = player.getPosition() + player.getVelocity() * 0.3;
}

void Bat::explore(World & world)
{
	if (isArrived()) {
		double radius = world.getRadius();
		double random01;
		do {
			random01 = (rand() / (RAND_MAX + 1.0f));
			double x = random01 * radius * 2.0f - radius;
			random01 = (rand() / (RAND_MAX + 1.0f));
			double z = random01 * radius * 2.0f - radius;
			target_position = { x, EXPLORE_HEIGHT, z };
		} while (target_position.getNorm() <= radius);
	}
}

void Bat::handleCollisions(Vector3& target)
{
	bool collide_height = false;
	bool collide_horizon = false;

	// check horizontal overlap
	float h_distance = (float)m_position.getDistanceXZ(target);
	if (h_distance < Player::RADIUS + RADIUS)
		collide_horizon = true;

	// check vertical overlap
	float v_distance = (float)abs(m_position.y - target.y);
	if (v_distance < Player::HALF_HEIGHT + HALF_HEIGHT)
		collide_height = true;

	// combine both horizontal and vertical result
	if (collide_height && collide_horizon)
		Collide();
}

void Bat::Collide()
{
	is_dead = true;
	hit_velocity = { m_velocity.x, 0.0, m_velocity.z };
	m_velocity = Vector3(0.0, 0.0, 0.0);
	m_acceleration = Vector3(0.0, 0.0, 0.0);
	not_hit_count = DeltaTime::FRAME_PER_SECOND;
}
