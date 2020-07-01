//
//	NoiseField.cpp
//

#include "NoiseField.h"

using namespace ObjLibrary;

NoiseField::NoiseField(float g, float a)
{
	grid_size = g;
	amplitude = a;
	for (int i = 0; i < 7; i++)
		seed[i] = (unsigned int)((rand() 
			/ (double)(RAND_MAX + 1.0)) * UINT_MAX);
}

float NoiseField::valueNoise(float x, float y)
{
	// calculate noise here
	int x0 = (int)(floor(x / grid_size));
	int y0 = (int)(floor(y / grid_size));
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	unsigned int value00 = pseudorandom(x0, y0);
	unsigned int value01 = pseudorandom(x0, y1);
	unsigned int value10 = pseudorandom(x1, y0);
	unsigned int value11 = pseudorandom(x1, y1);

	float x_frac = x / grid_size - x0;
	float y_frac = y / grid_size - y0;

	float x_fade1 = fade(x_frac);
	float y_fade1 = fade(y_frac);
	float x_fade0 = 1.0f - x_fade1;
	float y_fade0 = 1.0f - y_fade1;

	unsigned int value0 = (unsigned int)(value00 * y_fade0) +
		(unsigned int)(value01 * y_fade1);
	unsigned int value1 = (unsigned int)(value10 * y_fade0) +
		(unsigned int)(value11 * y_fade1);
	unsigned int value = (unsigned int)(value0 * x_fade0) +
		(unsigned int)(value1 * x_fade1);

	float value_scaled = ((float)(value) / UINT_MAX) * 2.0f - 1.0f;
	return value_scaled * amplitude;
}

float NoiseField::perlinNoise(float x, float y)
{
	// calculate noise here
	int x0 = (int)(floor(x / grid_size));
	int y0 = (int)(floor(y / grid_size));
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	float x_frac = x / grid_size - x0;
	float y_frac = y / grid_size - y0;

	float x_fade1 = fade(x_frac);
	float y_fade1 = fade(y_frac);
	float x_fade0 = 1.0f - x_fade1;
	float y_fade0 = 1.0f - y_fade1;

	Vector2 lattice00 = lattice(x0, y0);
	Vector2 lattice01 = lattice(x0, y1);
	Vector2 lattice10 = lattice(x1, y0);
	Vector2 lattice11 = lattice(x1, y1);

	Vector2 direction00 (-x_frac, -y_frac);
	Vector2 direction01 (-x_frac, 1.0f - y_frac);
	Vector2 direction10 (1.0f - x_frac, -y_frac);
	Vector2 direction11 (1.0f - x_frac, 1.0f - y_frac);

	float value00 = (float)lattice00.dotProduct(direction00);
	float value01 = (float)lattice01.dotProduct(direction01);
	float value10 = (float)lattice10.dotProduct(direction10);
	float value11 = (float)lattice11.dotProduct(direction11);

	float value0 = value00 * y_fade0 +
		value01 * y_fade1;
	float value1 = value10 * y_fade0 +
		value11 * y_fade1;
	float value = value0 * x_fade0 +
		value1 * x_fade1;

	return value * amplitude;
}

unsigned int NoiseField::pseudorandom(int x, int y)
{
	unsigned int n = (seed[0] * x) + (seed[1] * y);
	unsigned int quad_term = seed[2] * n * n +
		seed[3] * n + seed[4];
	return quad_term + (seed[5] * x) + (seed[6] * y);
}

float NoiseField::fade(float n)
{
	return (1 - cos(n * PI)) * 0.5f;
}

Vector2 NoiseField::lattice(int x, int y)
{
	unsigned int value = pseudorandom(x, y);
	float radians = value * PI;
	Vector2 result;
	result.x = cos(radians);
	result.y = sin(radians);
	return result;
}