//
//Player.cpp
//

#include "GetGlut.h"
#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"
#include "Player.h"
#include "World.h"
#include "Disk.h"
#include "DeltaTime.h"
#include "DiskType.h"
#include "Animation.h"

using namespace std;
using namespace ObjLibrary;
namespace {
	const Vector3 VELOCITY_DEFAULT(0.0, 0.0, 0.0);
	const Vector3 ACCELERATION_DEFAULT(0.0, 0.0, 0.0);
	const double  MOVE_RATE = 25.0;  // meters per second square
	const double  STRAFE_RATE = 10.0;  // meters per second square
	const double  TURN_RATE = 2.0;  // radians per second

	bool g_is_initialized = false;
	Animation g_animation;
}

const float   Player::HALF_HEIGHT = 0.8f;
const float   Player::RADIUS = 0.25f;
const float	  Player::HALF_PI = 0.5f * PI;
const float   Player::TWO_PI = 2.0f * PI;

void Player::init(const World& world)
{
	int number_of_disks = world.getDiskCount();
	float random01 = (rand() / (RAND_MAX + 1.0f));
	int disk_number = (int)floor(random01 * (float)number_of_disks);

	m_position = world.getDisk(disk_number).getPosition() + Vector3(1.0, 0.0, 1.0);
	m_forward = -m_position.getNormalized();

	m_velocity = VELOCITY_DEFAULT;
	m_acceleration = ACCELERATION_DEFAULT;
	jumping = false;

	updateY(world);
}

void Player::update(const World& world)
{	// Suggested Approach of jumping
	const Disk& player_disk = world.getClosestDisk(m_position);

	Vector3 previous_position = m_position;
	bool previous_on_disk = world.isCylinderOnAnyDisk(m_position, RADIUS);

	m_position += m_velocity * DeltaTime::FIXED_DELTA_TIME;
	bool now_on_disk = world.isCylinderOnAnyDisk(m_position, RADIUS);

	if (jumping) {// case 3 and 4
		m_velocity += Vector3(0.0, -9.8, 0.0) * DeltaTime::FIXED_DELTA_TIME;
		float surface = player_disk.getHeight(m_position);
		if (m_position.y - HALF_HEIGHT <= surface + 0.01 && now_on_disk)
		{// case 4
			if (previous_on_disk) {
				// case 4a
				updateY(world);
				jumping = false;
				m_velocity.y = 0.0;
			}
			else {// case 4b
				m_velocity.x = 0.0;
				m_velocity.z = 0.0;
				m_position = previous_position;
			}
		}// case 3: do nothing
	}
	else {// case 1 and 2
		if (now_on_disk) {// case 1
			updateY(world);
			m_velocity += m_acceleration * DeltaTime::FIXED_DELTA_TIME;

			unsigned int disk_type = player_disk.getDiskType();
			m_velocity *= pow(DiskType::getFrictionFactor(disk_type), DeltaTime::FIXED_DELTA_TIME);

			slidingAcceleration(player_disk);
			g_animation.decreaseRunCounter();
		}
		else {// case 2
			jumping = true;
			g_animation.resetCount();
		}
	}
	m_acceleration = ACCELERATION_DEFAULT;
}

void Player::updateY(const World& world)
{
	const Disk&  closest_disk = world.getClosestDisk(m_position);

	if(m_position.getDistanceXZ(closest_disk.getPosition()) <= RADIUS + closest_disk.getRadius())
		m_position.y = closest_disk.getHeight(m_position) + HALF_HEIGHT;
}

void Player::draw()
{
	if (!isModelsLoaded())
		loadModel();

	glPushMatrix();
	float rotation = (float)(m_forward.getRotationY() * 360 / TWO_PI - 90.0);
	glTranslated(m_position.x, m_position.y, m_position.z);
	glRotated(rotation, 0.0, 1.0, 0.0);

	g_animation.draw(jumping);

	glPopMatrix();
}

void Player::jump()
{
	if (!jumping) { // prevent double jump
		m_velocity = Vector3(m_forward.x * 8.0f, 12.0f, m_forward.z * 8.0f);
		jumping = true;
		g_animation.resetCount();
	}
}

void Player::isAttacked(ObjLibrary::Vector3 velocity)
{
	m_velocity = velocity;
	m_velocity.y = 5.0;
	m_acceleration = ACCELERATION_DEFAULT;
	jumping = true;
	g_animation.resetCount();
}

void Player::moveForward(const World& world)
{
	unsigned int disk_type = world.getClosestDisk(m_position).getDiskType();
	m_acceleration += m_forward * MOVE_RATE * DiskType::getAccelerationFactor(disk_type);
	g_animation.markRunning(true);
}

void Player::moveBackward(const World& world)
{
	unsigned int disk_type = world.getClosestDisk(m_position).getDiskType();
	m_acceleration -= m_forward * MOVE_RATE * DiskType::getAccelerationFactor(disk_type);
	g_animation.markRunning(false);
}

void Player::moveLeft(const World& world)
{
	unsigned int disk_type = world.getClosestDisk(m_position).getDiskType();
	m_acceleration += m_forward.getRotatedY(HALF_PI) * STRAFE_RATE 
		* DiskType::getAccelerationFactor(disk_type);
}

void Player::moveRight(const World& world)
{
	unsigned int disk_type = world.getClosestDisk(m_position).getDiskType();
	m_acceleration -= m_forward.getRotatedY(HALF_PI) * STRAFE_RATE 
		* DiskType::getAccelerationFactor(disk_type);
}

void Player::turnLeft()
{
	m_forward.rotateY(TURN_RATE * DeltaTime::FIXED_DELTA_TIME);
}

void Player::turnRight()
{
	m_forward.rotateY(-TURN_RATE * DeltaTime::FIXED_DELTA_TIME);
}

void Player::loadModel()
{
	assert(!isModelsLoaded());

	g_is_initialized = true;

	g_animation.loadModel();

	assert(isModelsLoaded());
}

bool Player::isModelsLoaded() const
{
	return g_is_initialized;
}

Vector3 Player::getPosition()
{
	return m_position;
}

Vector3 Player::getForward()
{
	return m_forward;
}

ObjLibrary::Vector3 Player::getVelocity()
{
	return m_velocity;
}

void Player::slidingAcceleration(const Disk& player_disk)
{
	unsigned int disk_type = player_disk.getDiskType();

	//compute minheight
	double min_height = m_position.y;
	Vector3 min_direction(0.0, 0.0, 0.0);
	for (int i = 0; i < 60; i++) {
		Vector3 direction = m_forward.getRotatedY(TWO_PI / 60.0 * (double)i);
		Vector3 position = m_position + 0.01f * direction;
		double height = player_disk.getHeight(position) + HALF_HEIGHT;
		if (min_height > height) {
			min_height = height;
			min_direction = direction;
		}
	}

	//compute sliding acceleration
	float slope = 0.0f;
	if (m_position.y > min_height)
		slope = (float)(m_position.y - min_height) / 0.01f;
	float min_slope = DiskType::getMinSlope(disk_type);
	float acceleration = 0.0f;
	if (slope > min_slope)
		acceleration = 10.0f * (slope - min_slope);

	m_velocity += min_direction * acceleration * DeltaTime::FIXED_DELTA_TIME;
}