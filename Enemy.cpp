//
//	Enemy.cpp
//

#include "Enemy.h"

using namespace std;
using namespace ObjLibrary;
namespace {
	const float BAT_INIT_HEIGHT = 15.0f;
	unsigned int hit_count = 0;
}

void Enemy::init(World & g_world)
{
	float radius = g_world.getRadius();
	int number = g_world.getDiskCount();
	for (int i = 1; i <= number; i++) {
		double rho = radius * sqrt((double)i / (double)number);
		double theta = PI * (3.0 - sqrt(5.0)) * ((double)i - 1.0);
		m_bats.push_back(Vector3(rho * cos(theta), BAT_INIT_HEIGHT, rho * sin(theta)));
	}
	m_world = g_world;
}

void Enemy::update(Player& player)
{
	for (int i = 0; i < (int)m_bats.size(); i++) {
		m_bats[i].update(m_world, player);
		if (m_bats[i].isDead()) {
			updateList(m_bats[i].getHitVelocity());
		}
	}
}

void Enemy::draw()
{
	for (int i = 0; i < m_world.getDiskCount(); i++) {
		m_bats[i].draw();
	}
	// draw white line for ring 0
	m_bats[0].drawLine();
}

bool Enemy::isMoreHit()
{
	if (hit_count != velocityList.size())
		return true;
	else
		return false;
}

ObjLibrary::Vector3 Enemy::getHitVelocity()
{
	hit_count++;
	return velocityList.back();
}

void Enemy::updateList(Vector3 velocity)
{
	if (velocityList.empty()||find(velocityList.begin(), velocityList.end(), velocity) == velocityList.end())
		velocityList.push_back(velocity);
}
