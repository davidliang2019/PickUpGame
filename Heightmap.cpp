//
//	Heightmap.cpp
//

#include <algorithm>
#include "Heightmap.h"
#include "DiskType.h"

using namespace std;
using namespace ObjLibrary;
namespace {
	float randomInterval(float min, float max)
	{
		float random01 = (rand() / (RAND_MAX + 1.0f));
		return (random01 * (max - min) + min);
	}
}

Heightmap::Heightmap(int diskType)
	: m_diskType(diskType)
	, m_heightmap_size(DiskType::getSideLength(m_diskType))
	, m_repeat(DiskType::getTexureRepeatCount(m_diskType))
	, m_heights(m_heightmap_size * m_heightmap_size, 0.0f)

{
	initHeightmapHeights();
	initHeightmapDisplayList();
}

Heightmap::Heightmap()
	: m_diskType(), m_heightmap_size(0), m_repeat(0) {}

void Heightmap::draw()
{
	heightmap_list.draw();
}

void Heightmap::initHeightmapHeights()
{
	switch (m_diskType) {
	case DiskType::RED_ROCK: redRockGenerator();
		break;
	case DiskType::LEAFY: leafyGenerator();
		break;
	case DiskType::ICY: icyGenerator();
		break;
	case DiskType::SANDY: sandyGenerator();
		break;
	case DiskType::GREY_ROCK: greyRockGenerator();
		break;
	default: exit(1);
	}
}

void Heightmap::initHeightmapDisplayList()
{
	heightmap_list.begin();
	glEnable(GL_TEXTURE_2D);
	TextureManager::activate(DiskType::getTextureName(m_diskType));
	glColor3d(1.0, 1.0, 1.0);
	for (int x0 = 0; x0 < m_heightmap_size; x0++)
	{
		unsigned int x1 = x0 + 1;
		float tex_x0 = (float)(x0) / m_heightmap_size * m_repeat;
		float tex_x1 = (float)(x1) / m_heightmap_size * m_repeat;
		glBegin(GL_TRIANGLE_STRIP);
		for (int z = 0; z <= m_heightmap_size; z++)
		{
			float tex_z = (float)(z) / m_heightmap_size * m_repeat;
			glTexCoord2d(tex_x1, tex_z);
			glVertex3d(x1, m_heights[indexing(x1 % m_heightmap_size, z % m_heightmap_size)], z);
			glTexCoord2d(tex_x0, tex_z);
			glVertex3d(x0, m_heights[indexing(x0 % m_heightmap_size, z % m_heightmap_size)], z);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	heightmap_list.end();
}

void Heightmap::redRockGenerator()
{
	float number = 0.0f;
	for (int i = 2; i <= m_heightmap_size/2; i++) {
		number += randomInterval(-1.0, 2.0);
		for (int j = i; j<= m_heightmap_size - i; j++) {
			m_heights[indexing(i,j)] = number;
			m_heights[indexing(j,i)] = number;
			m_heights[indexing(m_heightmap_size-i,j)] = number;
			m_heights[indexing(j,m_heightmap_size-i)] = number;
		}
	}
}

void Heightmap::leafyGenerator()
{
	float number[11];
	leafyPreprocess(number);

	for (int x = 1; x < m_heightmap_size; x++)
		for (int z = 1; z < m_heightmap_size; z++)
			m_heights[indexing(x,z)] = leafyHeight((float)x
				/ m_heightmap_size, (float)z / m_heightmap_size, number);
}

void Heightmap::icyGenerator()
{
	Vector3 random_points[200];
	for (int j = 0; j < 200; j++) {
		float temp, d = 0.0f, h = m_heightmap_size / 2.0f;
		for (int i = 0; i < 4; i++) {
			temp = randomInterval(0, h);
			if (d < temp)
				d = temp;
		}
		float m = (h - d) * 0.7f;
		float angle = randomInterval(0.0f, 2.0f * PI);
		float y = randomInterval(m * (-1.0f), m);
		random_points[j].x = h + cos(angle) * d;
		random_points[j].y = y;
		random_points[j].z = h + sin(angle) * d;
	}

	for (int x = 1; x < m_heightmap_size; x++)
		for (int z = 1; z < m_heightmap_size; z++)
			m_heights[indexing(x,z)] = icyHeight((float)x, (float)z, random_points);
}

void Heightmap::sandyGenerator()
{
	NoiseField noiseObject(16.0f, 8.0f);
	for (int x = 1; x < m_heightmap_size; x++)
		for (int z = 1; z < m_heightmap_size; z++)
			m_heights[indexing(x,z)] = noiseObject.perlinNoise((float)x, (float)z)
			* edgeFactor((float)x / m_heightmap_size, (float)z / m_heightmap_size);
}

void Heightmap::greyRockGenerator()
{
	NoiseField noiseLayer0(32.0f, 10.0f);
	NoiseField noiseLayer1(16.0f, 7.0f);
	NoiseField noiseLayer2(8.0f, 5.0f);
	NoiseField noiseLayer3(4.0f, 3.5f);
	NoiseField noiseLayer4(2.0f, 2.5f);
	for (int x = 1; x < m_heightmap_size; x++)
		for (int z = 1; z < m_heightmap_size; z++)
			m_heights[indexing(x,z)] = (noiseLayer0.perlinNoise((float)x, (float)z)
			+ noiseLayer1.perlinNoise((float)x, (float)z)
			+ noiseLayer2.perlinNoise((float)x, (float)z)
			+ noiseLayer3.perlinNoise((float)x, (float)z)
			+ noiseLayer4.perlinNoise((float)x, (float)z))
			* edgeFactor((float)x / m_heightmap_size, (float)z / m_heightmap_size);

}

void Heightmap::leafyPreprocess(float number[])
{
	for (int i = 0; i < 6; i++) {
		number[i] = randomInterval(-1.0, 1.0);
	}
	int one = rand() % 7;
	int two = rand() % 7;
	number[6] = (float)min(one, two);
	number[7] = randomInterval(0.0f, 2.0f * PI);
	float sign;
	if (rand() % 2)
		sign = 1.0f;
	else
		sign = -1.0f;
	for (int i = 0; i < 3; i++)
		number[8 + i] = randomInterval(0.0, 1.0) * sign;
}

float Heightmap::leafyHeight(float x, float y, float number[])
{
	//set up all random numbers
	float LL = number[0];
	float LC = number[1];
	float LR = number[2];
	float RI = number[3];
	float RM = number[4];
	float RO = number[5];
	int ARM_COUNT = (int)number[6];
	float ARM_RADIANS = number[7];
	float AI = number[8];
	float AM = number[9];
	float AO = number[10];

	//using the code in the assignment2 webpage
	float x2 = x * 2.0f - 1.0f;
	float y2 = y * 2.0f - 1.0f;
	float dist = min(sqrt(x2*x2 + y2*y2), 1.0f);
	float radians = atan2(y2, x2);
	float ll = x*x*x*x*(1.0f - x)*12.0f;
	float lc = (cos(PI * x2) + 1.0f) * 0.5f;
	float lr = (1.0f - x)*(1.0f - x)*(1.0f - x)*(1.0f - x) * x * 12.0f;
	float lsum = ll * LL + lc * LC + lr * LR;

	float ri = max((1.0f - dist)*(1.0f - dist), 0.0f);
	float rm = sin(PI * min(pow(dist, 0.8f) * 4.0f / 3.0f, 1.0f));
	float ro = max(sin(PI * pow(dist, 1.6f)), 0.0f);
	float rsum = ri * RI + rm * RM + ro * RO;

	float ai = (sqrt(dist) - dist) * 4.0f;
	float am = dist * dist * (1.0f - dist) * (1.0f - dist) * 16.0f;
	float ao = (sqrt(1.0f - dist) - (1.0f - dist)) * 4;
	float asum = ai * AI + am * AM + ao * AO;

	float non_arm_height = lsum * rsum;
	float arm_radians = radians * ARM_COUNT + ARM_RADIANS;
	float arm_magnitude = (sin(arm_radians) + 1.0f) * 0.5f;
	float arm_height = arm_magnitude * asum;

	return non_arm_height * 5.0f + arm_height * 3.0f;
}

float Heightmap::icyHeight(float x, float z, Vector3 points[])
{
	float minHeight = 0.0f, maxHeight = 0.0f;
	for (int i = 0; i < 200; i++) {
		float horDist = 0.7f * (float)sqrt((x - points[i].x)*(x - points[i].x)
			+ (z - points[i].z)*(z - points[i].z));
		float p = (float)points[i].y - horDist;
		float n = (float)points[i].y + horDist;
		if (p > maxHeight)
			maxHeight = p;
		if (n < minHeight)
			minHeight = n;
	}
	return maxHeight + minHeight;
}

float Heightmap::edgeFactor(float  x, float y)
{
	return (1 - max(max(x*x*x*x*x*x, 
		(1.0f - x)*(1.0f - x)*(1.0f - x)*(1.0f - x)*(1.0f - x)*(1.0f - x)),
		max(y*y*y*y*y*y, 
		(1.0f - y)*(1.0f - y)*(1.0f - y)*(1.0f - y)*(1.0f - y)*(1.0f - y))))
		/ (1.0f - 0.5f*0.5f*0.5f*0.5f*0.5f*0.5f);
}

// add a new function for Assignment 3
// using the method in Hand out 4-1
float Heightmap::getHeight(float x, float z) const
{
	int i = (int)floor(x);
	int k = (int)floor(z);
	int h = m_heightmap_size;

	float i_frac = x - i;
	float k_frac = z - k;

	float height;
	if (i_frac > k_frac) {
		float w1 = 1.0f - i_frac;
		float w2 = k_frac;
		float w0 = 1.0f - w1 - w2;

		height = w0 * m_heights[indexing((i + 1) % h, k)]
			+ w1 * m_heights[indexing(i, k)]
			+ w2 * m_heights[indexing((i + 1) % h, (k + 1) % h)];
	}
	else {
		float w2 = i_frac;
		float w1 = 1.0f - k_frac;
		float w3 = 1.0f - w1 - w2;

		height = w1 * m_heights[indexing(i, k)]
			+ w2 * m_heights[indexing((i + 1) % h, (k + 1) % h)]
			+ w3 * m_heights[indexing(i, (k + 1) % h)];
	}
	return height;
}

